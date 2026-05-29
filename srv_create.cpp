#include "bircd.hpp"

void	env::set_non_blocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl error (" << __FILE__ << ", " << __LINE__ << "): " << strerror(errno) << std::endl;
		close(fd);
		exit(1);
	}
}

void			env::srv_create()
{
	int			s;
	sockaddr_in	sin;
	protoent	*pe;
  
	pe = getprotobyname("tcp");
	if (pe == NULL)
	{
		std::cerr << "getprotobyname error (" << __FILE__ << ", " << __LINE__ << "): " << strerror(errno) << std::endl;
		exit(1);
	}
	s = socket(PF_INET, SOCK_STREAM, pe->p_proto);
	if (s == -1)
	{
		std::cerr << "socket error (" << __FILE__ << ", " << __LINE__ << "): " << strerror(errno) << std::endl;
		exit(1);
	}
	set_non_blocking(s);


	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	if (bind(s, reinterpret_cast<sockaddr*>(&sin), sizeof(sin)) == -1)
	{
		std::cerr << "bind error (" << __FILE__ << ", " << __LINE__ << "): " << strerror(errno) << std::endl;
		exit(1);
	}
	if (listen(s, 42) == -1)
	{
		std::cerr << "listen error (" << __FILE__ << ", " << __LINE__ << "): " << strerror(errno) << std::endl;
		exit(1);
	}
	fds[s].type = FD_SERV;
	fds[s].fct_read = &env::srv_accept;

	epoll_add(s, EPOLLIN);
}
