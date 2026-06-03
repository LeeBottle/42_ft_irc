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

	// 2. 전체 클라이언트 소켓의 출력 버퍼 상태 일괄 스캔 및 정밀 동기화
	for (size_t i = 0; i < fds.size(); ++i)
	{
		if (fds[i].type == FD_FREE || fds[i].type == FD_SERV)
		{
			continue;
		}

		// 보낼 데이터가 남아있거나, 강제 종료 플래그가 켜진 세션은 EPOLLOUT을 활성화하여
		// client_write 함수 내부로 제어 흐름이 들어가 최종 폭파되도록 유도합니다.
		if (!fds[i].buf_write.empty() || fds[i].close_after_write)
		{
			struct epoll_event ev;
			ev.events = EPOLLIN | EPOLLOUT;
			ev.data.fd = static_cast<int>(i);
			epoll_ctl(epoll_fd, EPOLL_CTL_MOD, static_cast<int>(i), &ev);
		}
		else
		{
			struct epoll_event ev;
			ev.events = EPOLLIN;
			ev.data.fd = static_cast<int>(i);
			epoll_ctl(epoll_fd, EPOLL_CTL_MOD, static_cast<int>(i), &ev);
		}
	}
}