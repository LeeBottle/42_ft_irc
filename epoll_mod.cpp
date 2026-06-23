#include "bircd.hpp"

void env::epoll_mod(int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) < 0)
	{
		std::cerr << "epoll_ctl MOD error: " << strerror(errno) << std::endl;
	}
}
