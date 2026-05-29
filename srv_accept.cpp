#include "bircd.hpp"

void	env::srv_accept(int s)
{
	int         cs;
	sockaddr_in csin;
	socklen_t   csin_len;

	csin_len = sizeof(csin);
	cs = accept(s, reinterpret_cast<sockaddr*>(&csin), &csin_len);
	if (cs == -1)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
		{
			std::cerr << "accept error (" << __FILE__ << ", " << __LINE__ << "): " << strerror(errno) << std::endl;
		}
		return;
	}
	set_non_blocking(cs);
	
	std::cout << "New client #" << cs << " from " 
			<< inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << std::endl;

	fds[cs].clean_fd();
	fds[cs].type = FD_FREE;
	fds[cs].fct_read = &env::client_read;
	fds[cs].fct_write = &env::client_write;
	
	epoll_add(cs, EPOLLIN); 
}
