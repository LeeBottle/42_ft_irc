#include "bircd.hpp"

std::string	env::client_read_line(int cs)
{
	size_t pos;

	pos = fds[cs].buf_read.find('\n');
	if (pos == std::string::npos)
	{
		return "";
	}

	std::string cmd_line = fds[cs].buf_read.substr(0, pos);

	fds[cs].buf_read.erase(0, pos + 1);

	if (!cmd_line.empty() && cmd_line[cmd_line.size() - 1] == '\r')
	{
		cmd_line.erase(cmd_line.size() - 1);
	}

	return cmd_line;
}

void	env::client_read(int cs)
{
	char		buf[BUF_SIZE + 1];
	ssize_t	r;

	r = read(cs, buf, BUF_SIZE);
	if (r == 0)
	{
		std::cout << "Client #" << cs << " disconnected." << std::endl;
		std::map<std::string, Channel>::iterator it = channels.begin();
		while (it != channels.end())
		{
			// channels.erase(it)가 실행되면 iterator가 파괴되므로, 다음 iterator를 미리 안전하게 확보합니다.
			std::map<std::string, Channel>::iterator next_it = it;
			++next_it;
			
			// 해당 클라이언트가 이 채널의 소속 멤버인지 검사합니다.
			if (it->second.is_member(cs))
			{
				std::cout << "Forced removing Client #" << cs << " from channel: " << it->second.get_name() << std::endl;
				
				// 채널 객체 내부에서 클라이언트를 제거합니다. (내부에서 오퍼레이터 권한도 함께 자동 삭제됨)
				it->second.remove_client(cs);
				
				// 만약 이 유저가 방을 나감으로써 채널에 남은 인원이 한 명도 없다면, 채널 자체를 소멸시킵니다.
				if (it->second.get_client_fds().empty())
				{
					std::cout << "Channel " << it->second.get_name() << " became empty. Dissolving channel resource." << std::endl;
					channels.erase(it);
				}
				else if (it->second.get_operator_fds().empty())
				{
					// [선택 항목/보안 강화] 만약 방장(Operator)이 비정상 종료되어 방에 오퍼레이터가 없다면,
					// 남은 유저 중 첫 번째 유저에게 방장 권한을 자동으로 이양하고 공지 패킷을 누적합니다.
					int next_op_fd = it->second.get_client_fds()[0];
					it->second.add_operator(next_op_fd);
					
					std::string op_msg = ":ircserv MODE " + it->second.get_name() + " +o " + fds[next_op_fd].nickname + "\r\n";
					
					std::vector<int> members = it->second.get_client_fds();
					for (size_t m = 0; m < members.size(); ++m)
					{
						fds[members[m]].buf_write += op_msg;
					}
				}
			}
			it = next_it; // 안전하게 다음 채널 검사로 이동
		}
		epoll_del(cs);
		close(cs);
		fds[cs].clean_fd();
	}
	if (r < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
		{
			return;
		}
		std::cerr << "read error (" << __FILE__ << ", " << __LINE__ << "): " << strerror(errno) << std::endl;
		epoll_del(cs);
		close(cs);
		fds[cs].clean_fd();
		return;
	}

	buf[r] = '\0';
	std::cout << "\n┌- [RAW DATA] Received from #" << cs << " (Bytes: " << r << ") -┐" << std::endl;
	std::cout << buf;
	std::cout << "└------------------------------------------┘\n" << std::endl;
	fds[cs].buf_read += buf;

	while (true)
	{
		if (fds[cs].close_after_write)
			break;

		std::string cmd_line = client_read_line(cs);

		if (cmd_line.empty())
			break;

		std::cout << "Process Line from #" << cs << ": " << cmd_line << std::endl;

		handle_commands(cs, cmd_line);
	}
	if (!fds[cs].buf_write.empty() || fds[cs].close_after_write)
	{
		//client_write(cs);
		epoll_mod(cs, EPOLLIN | EPOLLOUT);
	}
}
