#include "bircd.hpp"

void	env::client_read(int cs)
{
	int	r;
	int	i;

	char	buf[BUF_SIZE + 1];

	std::memset(buf, 0, sizeof(buf));
	r = recv(cs, buf, BUF_SIZE, 0);
	if (r <= 0)
	{
		close(cs);
		fds[cs].clean_fd();
		printf("client #%d gone away\n", cs);
	}
	else
	{
		fds[cs].buf_read = std::string(buf, r);
		i = 0;
		while (i < maxfd)
		{
			if ((fds[i].type == FD_CLIENT) && (i != cs))
				send(i, fds[cs].buf_read.data(), fds[cs].buf_read.size(), 0);
			i++;
		}
    }
}
