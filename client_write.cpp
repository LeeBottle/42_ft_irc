#include "bircd.hpp"

void	env::client_write(int cs)
{
	if (!fds[cs].buf_write.empty())
	{
		ssize_t sent = write(cs, fds[cs].buf_write.c_str(), fds[cs].buf_write.size());
		
		if (sent < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			{
				return;
			}
			std::cerr << "write error (" << __FILE__ << ", " << __LINE__ << "): " << strerror(errno) << std::endl;
			epoll_del(cs);
			close(cs);
			fds[cs].clean_fd();
			return;
		}
		if (sent == 0)
		{
			return;
		}

		fds[cs].buf_write.erase(0, sent);
	}

	// 데이터 전송 완료 후 최종 상태 점검
	if (fds[cs].buf_write.empty())
	{
		if (fds[cs].close_after_write)
		{
			std::cout << "Session closed for Client #" << cs << " due to authentication failure." << std::endl;
			epoll_del(cs);
			close(cs);
			fds[cs].clean_fd();
			return;
		}
		
		//버퍼가 완전히 비었으므로 EPOLLOUT(쓰기감시)을 끄고 EPOLLIN(읽기)
		epoll_mod(cs, EPOLLIN);
	}
}
