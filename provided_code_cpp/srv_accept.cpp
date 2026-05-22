
#include "bircd.hpp"

void			env::srv_accept(int s)
{
	int			cs;
	sockaddr_in	csin;
	socklen_t		csin_len;

	csin_len = sizeof(csin);
	cs = accept(s, reinterpret_cast<sockaddr*>(&csin), &csin_len);
	if (cs == -1)
	{
		fprintf(stderr, "accept error (%s, %d): %s\n", __FILE__, __LINE__, strerror(errno));
		exit(1);
	}
	printf("New client #%d from %s:%d\n", cs,
		inet_ntoa(csin.sin_addr), ntohs(csin.sin_port));
	fds[cs].clean_fd();
	fds[cs].type = FD_CLIENT;
	fds[cs].fct_read = &env::client_read;
	fds[cs].fct_write = &env::client_write;
}
