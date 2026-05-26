#include "bircd.hpp"

void	env::inifd()
{
	int	i;

	i = 0;
	max = 0;
	FD_ZERO(&fd_read);
	FD_ZERO(&fd_write);
	while (i < maxfd)
	{
		if (fds[i].type != FD_FREE)
		{
			FD_SET(i, &fd_read);
			if (!fds[i].buf_write.empty())
			{
				FD_SET(i, &fd_write);
			}
			max = std::max(max, i);
		}
		i++;
	}
}
