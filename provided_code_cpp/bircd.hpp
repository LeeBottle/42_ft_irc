#ifndef BIRCD_H_
# define BIRCD_H_

// C++ 표준 라이브러리 헤더
# include <algorithm>
# include <iostream>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <cerrno>
# include <new>
# include <vector>

// 시스템 및 네트워크 관련 헤더
# include <sys/select.h>
# include <sys/socket.h>
# include <sys/resource.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>

# define FD_FREE	0
# define FD_SERV	1
# define FD_CLIENT	2

# define BUF_SIZE	4096


class env;

class fd
{
public:
	int			type;
	void		(env::*fct_read)(int);
	void		(env::*fct_write)(int);
	std::string	buf_read;
	std::string	buf_write;

	fd() : type(0), fct_read(NULL), fct_write(NULL), buf_read(""), buf_write("") {}
	~fd() {}

	void	clean_fd();
};


class env
{
public:
	std::vector<fd>	fds;
	int		port;
	int		maxfd;
	int		max;
	int		r;
	fd_set	fd_read;
	fd_set	fd_write;

	env() :  port(0), maxfd(0), max(0), r(0)
	{
		FD_ZERO(&fd_read);
		FD_ZERO(&fd_write);
	}
	~env() {}

	//
	void	init_env();

	//
	void	get_opt(int ac, char **av);

	//
	void	srv_create();
	void	srv_accept(int s);
	void	client_read(int cs);
	void	client_write(int cs);
	
	//
	void	main_loop();
	void	inifd();
	void	do_select();
	void	check_fd();
};


#endif