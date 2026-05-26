#include "bircd.hpp"

void	env::check_epoll()
{
	for (int i = 0; i < r; i++)
    {
        int fd = epoll_events[i].data.fd;
        uint32_t events = epoll_events[i].events;

        if (fds[fd].type == FD_SERV)
        {
            if ((events & EPOLLIN) && fds[fd].fct_read != NULL)
            {
                (this->*fds[fd].fct_read)(fd); 
            }
        }
        else
        {
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
}