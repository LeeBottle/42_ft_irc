#include "bircd.hpp"

void	env::check_fd()
{
  int	i;

  i = 0;
  while ((i < maxfd) && (r > 0))
    {
		if (FD_ISSET(i, &fd_read) && fds[i].fct_read != NULL)
			(this->*fds[i].fct_read)(i);
	
		if (FD_ISSET(i, &fd_write) && fds[i].fct_write != NULL)
		(this->*fds[i].fct_write)(i);
	
		if (FD_ISSET(i, &fd_read) || FD_ISSET(i, &fd_write))
			r--;
		i++;
    }
}
