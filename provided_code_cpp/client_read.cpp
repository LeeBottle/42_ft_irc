#include "bircd.hpp"

void	env::client_read(int cs)
{
	char	buf[BUF_SIZE + 1];
	int		r;

	r = read(cs, buf, BUF_SIZE);
	if (r <= 0)
	{
		std::cout << "Client #" << cs << " disconnected." << std::endl;
		epoll_del(cs);
		close(cs);
		fds[cs].clean_fd();
	}
	else
	{
		buf[r] = '\0';
		
		std::cout << "Read " << r << " bytes: " << buf;
		if (buf[r - 1] != '\n') {
			std::cout << std::endl;
		}

		std::string check_str(buf);
		size_t pass_pos = check_str.find("PASS");
		
		if (pass_pos != std::string::npos)
		{
			std::string raw_extract = check_str.substr(pass_pos + 5);
			size_t newline_pos = raw_extract.find('\n');
			std::string input_pw = (newline_pos != std::string::npos) ? raw_extract.substr(0, newline_pos) : raw_extract;

			if (!input_pw.empty() && input_pw[input_pw.size() - 1] == '\r')
			{
				input_pw.erase(input_pw.size() - 1);
			}

			if (input_pw == password)
			{
				std::cout << "Client #" << cs << " successfully authorized." << std::endl;
				fds[cs].buf_write += ":ircserv NOTICE * :Password authorized. Welcome!\r\n";
				epoll_mod(cs, EPOLLIN | EPOLLOUT);
			}
			else
			{
				std::cout << "Client #" << cs << " kicked (Incorrect password)." << std::endl;
				send(cs, ":ircserv 464 * :Password incorrect.\r\n", 37, 0);
				epoll_del(cs);
				close(cs);
				fds[cs].clean_fd();
				return;
			}
		}

		for (size_t i = 0; i < fds.size(); ++i)
		{
			if (fds[i].type == FD_CLIENT && static_cast<int>(i) != cs)
			{
				std::string msg = "[Client " + std::to_string(cs) + "]: " + buf;
				fds[i].buf_write += msg;
				epoll_mod(i, EPOLLIN | EPOLLOUT);
			}
		}
	}
}