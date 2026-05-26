#include "bircd.hpp"

void env::epoll_add(int fd, uint32_t events) {
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
		perror("epoll_ctl: ADD");
	}
}

void env::epoll_del(int fd) {
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr) < 0) {
		perror("epoll_ctl: DEL");
	}
}

void env::epoll_mod(int fd, uint32_t events) {
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) < 0) {
		perror("epoll_ctl: MOD");
	}
}