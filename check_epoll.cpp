#include "bircd.hpp"

void	env::check_epoll()
{
	// 1. 커널이 반환한 읽기/쓰기 이벤트 일괄 처리
	for (int i = 0; i < r; i++)
	{
		int fd = epoll_events[i].data.fd;
		uint32_t events = epoll_events[i].events;

		if (fds[fd].type == FD_SERV)
		{
			if ((events & EPOLLIN) && fds[fd].fct_read != NULL)
			{
				(this->*fds[fd].fct_read)(fd); 
			}
		}
		else
		{
			if ((events & EPOLLIN) && fds[fd].fct_read != NULL)
			{
				(this->*fds[fd].fct_read)(fd);
			}
			// 이미 끊어질 소켓이더라도 쓰기 이벤트가 들어오면 마저 전송 처리 진행
			if ((events & EPOLLOUT) && fds[fd].fct_write != NULL)
			{
				(this->*fds[fd].fct_write)(fd);
			}
		}
	}
}