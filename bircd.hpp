#ifndef BIRCD_H_
# define BIRCD_H_

// 입출력 및 문자열 처리
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
// 컨테이너 및 알고리즘
#include <vector>
#include <map>
#include <algorithm>
// 메모리 및 예외 처리
#include <new>
#include <cstdlib>
#include <cerrno>


// 파일 디스크립터 및 시스템 자원 관리
#include <unistd.h>
#include <fcntl.h>
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


#include "channel.hpp"


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
	std::string nickname;
    std::string username;
	bool		close_after_write;
	bool		is_welcomed;

	fd() : type(0), fct_read(NULL), fct_write(NULL), buf_read(""), buf_write(""), close_after_write(false) {}
	~fd() {}

	void	clean_fd();
};


class env
{
public:
	std::vector<fd>	fds;
	std::map<std::string, Channel>	channels;
	int		port;
	int		max;
	int		r;

	int								epoll_fd;
    std::vector<struct epoll_event> epoll_events;
	std::string	password;

	env() : port(0), epoll_fd(-1), max(0), r(0), password("") {}
	~env() {}

	//
	void	init_env();

	//
	void	get_opt(int ac, char **av, env &e);

	//
	void	srv_create();
	void	srv_accept(int s);
	void	set_non_blocking(int fd);
	
	void	client_read(int cs);
	std::string	client_read_line(int cs);
	void 	handle_commands(int cs, const std::string& cmd_line);
	void 	handle_command_pass(int cs, const std::string& cmd_line);
	void	broadcast_message(int sender_cs, const std::string& target, const std::string& message);
	void 	handle_command_nick(int cs, const std::string& cmd_line);
	void 	handle_command_user(int cs, const std::string& cmd_line);
	void	handle_command_privmsg(int cs, const std::string& cmd_line);
	
	void	handle_command_join(int cs, const std::string& cmd_line);
	void    handle_command_part(int cs, const std::string& cmd_line);
	void    handle_command_kick(int cs, const std::string& cmd_line);
	void    handle_command_invite(int cs, const std::string& cmd_line);
	void    handle_command_topic(int cs, const std::string& cmd_line);

	void    handle_command_mode(int cs, const std::string& cmd_line);

	void	client_write(int cs);
	
	//
	void	main_loop();
	void	inifd();
	void	do_epoll();

	void	epoll_add(int fd, uint32_t events);
	void	epoll_del(int fd);
	void	check_epoll();
};


#endif
