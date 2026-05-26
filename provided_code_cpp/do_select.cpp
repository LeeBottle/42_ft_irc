#include "bircd.hpp"

void	env::do_select()
{
	r = epoll_wait(epoll_fd, epoll_events.data(), epoll_events.size(), -1);

    if (r < 0)
    {
        if (errno != EINTR)
        {
            std::cerr << "epoll_wait error (" << __FILE__ << ", " << __LINE__ << "): " << strerror(errno) << std::endl;
            exit(1);
        }
        r = 0; 
    }
}
