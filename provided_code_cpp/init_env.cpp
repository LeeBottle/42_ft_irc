#include "bircd.hpp"

void		env::init_env()
{
	rlimit	rlp;

	if (getrlimit(RLIMIT_NOFILE, &rlp) == -1)
	{
		fprintf(stderr, "getrlimit error (%s, %d): %s\n", __FILE__, __LINE__, strerror(errno));
		exit(1);
	}
	maxfd = rlp.rlim_cur;
	fds.resize(maxfd);
}