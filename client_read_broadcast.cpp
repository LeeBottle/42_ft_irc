#include "bircd.hpp"

	void	env::broadcast_message(int sender_cs, const std::string& message)
	{
		for (size_t i = 0; i < fds.size(); ++i)
		{
			if (fds[i].type == FD_CLIENT && static_cast<int>(i) != sender_cs)
			{
				std::string msg;

				if (!fds[sender_cs].nickname.empty())
				{
					msg = "[" + fds[sender_cs].nickname + "]: " + message + "\r\n";
				}
				else
				{
					std::stringstream ss;
					ss << sender_cs;
					msg = "[Client " + ss.str() + "]: " + message + "\r\n";
				}
				fds[i].buf_write += msg;

				epoll_mod(i, EPOLLIN | EPOLLOUT);
			}
		}
	}
