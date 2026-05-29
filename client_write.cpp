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

	if (fds[cs].buf_write.empty())
	{
		epoll_mod(cs, EPOLLIN);
	}
}
