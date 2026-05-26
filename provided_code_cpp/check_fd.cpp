#include "bircd.hpp"

void	env::check_fd()
{
	for (int i = 0; i < r; i++)
	{
		int fd = epoll_events[i].data.fd;
		uint32_t events = epoll_events[i].events;

		if ((events & EPOLLIN) && fds[fd].fct_read != NULL)
		{
			(this->*fds[fd].fct_read)(fd);
		}

		if ((events & EPOLLOUT) && fds[fd].fct_write != NULL)
		{
			(this->*fds[fd].fct_write)(fd);
		}
	}
}
