#include "bircd.hpp"

void		env::init_env()
{
	rlimit	rlp;

	if (getrlimit(RLIMIT_NOFILE, &rlp) == -1)
	{
		fprintf(stderr, "getrlimit error (%s, %d): %s\n", __FILE__, __LINE__, strerror(errno));
		exit(1);
	}
	int max_connections = rlp.rlim_cur;

	fds.resize(max_connections);

	epoll_fd = epoll_create1(0);
	if (epoll_fd < 0) {
		perror("epoll_create1");
		exit(1);
	}
	
	epoll_events.resize(max_connections);
}