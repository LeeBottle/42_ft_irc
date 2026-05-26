#ifndef BIRCD_H_
# define BIRCD_H_

// 입출력 및 문자열 처리
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
// 컨테이너 및 알고리즘
#include <vector>
#include <algorithm>
// 메모리 및 예외 처리
#include <new>
#include <cstdlib>
#include <cerrno>


// 파일 디스크립터 및 시스템 자원 관리
#include <unistd.h>
#include <sys/resource.h>


// epoll 기반 이벤트 처리
#include <sys/epoll.h>
// select 기반 이벤트 처리
#include <sys/select.h>


// 소켓 기본 API
#include <sys/socket.h>
// 인터넷 주소 구조체 및 프로토콜
#include <netinet/in.h>
#include <arpa/inet.h>
// DNS 및 호스트 정보 조회
#include <netdb.h>

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
	int		max;
	int		r;

	int								epoll_fd;
    std::vector<struct epoll_event> epoll_events;
	std::string	password;

	env() : port(0), epoll_fd(-1), max(0), r(0) {}
	~env() {}

	//
	void	init_env();

	//
	void	get_opt(int ac, char **av, env &e);

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

	void	epoll_add(int fd, uint32_t events);
	void	epoll_del(int fd);
	void	epoll_mod(int fd, uint32_t events);
	void	check_epoll();
};


#endif