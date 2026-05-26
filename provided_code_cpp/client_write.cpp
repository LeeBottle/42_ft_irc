#include "bircd.hpp"

void	env::client_write(int cs)
{
	if (!fds[cs].buf_write.empty())
	{
		int sent = write(cs, fds[cs].buf_write.c_str(), fds[cs].buf_write.size());
		
		if (sent < 0)
		{
			perror("write error");
			epoll_del(cs);
			close(cs);
			fds[cs].clean_fd();
			return;
		}
		
		fds[cs].buf_write.erase(0, sent);
	}

	if (fds[cs].buf_write.empty())
	{
		epoll_mod(cs, EPOLLIN);
	}
}
