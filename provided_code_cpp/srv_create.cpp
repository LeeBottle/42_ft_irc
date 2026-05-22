#include "bircd.hpp"

void			env::srv_create()
{
	int			s;
	sockaddr_in	sin;
	protoent	*pe;
  
	pe = getprotobyname("tcp");
	if (pe == NULL)
	{
		fprintf(stderr, "getprotobyname error (%s, %d): %s\n", __FILE__, __LINE__, strerror(errno));
		exit(1);
	}
	s = socket(PF_INET, SOCK_STREAM, pe->p_proto);
	if (s == -1)
	{
		fprintf(stderr, "socket error (%s, %d): %s\n", __FILE__, __LINE__, strerror(errno));
		exit(1);
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	if (bind(s, reinterpret_cast<sockaddr*>(&sin), sizeof(sin)) == -1)
	{
		fprintf(stderr, "bind error (%s, %d): %s\n", __FILE__, __LINE__, strerror(errno));
		exit(1);
	}
	if (listen(s, 42) == -1)
	{
		fprintf(stderr, "listen error (%s, %d): %s\n", __FILE__, __LINE__, strerror(errno));
		exit(1);
	}
	fds[s].type = FD_SERV;
	fds[s].fct_read = &env::srv_accept;
}
