#include "bircd.hpp"

std::string	env::client_read_line(int cs)
{
	size_t pos;

	pos = fds[cs].buf_read.find('\n');
	if (pos == std::string::npos)
	{
		return "";
	}

	std::string cmd_line = fds[cs].buf_read.substr(0, pos);

	fds[cs].buf_read.erase(0, pos + 1);

	if (!cmd_line.empty() && cmd_line[cmd_line.size() - 1] == '\r')
	{
		cmd_line.erase(cmd_line.size() - 1);
	}

	return cmd_line;
}

void	env::client_read(int cs)
{
	char	buf[BUF_SIZE + 1];
	int		r;

	r = read(cs, buf, BUF_SIZE);
	if (r <= 0)
	{
		std::cout << "Client #" << cs << " disconnected." << std::endl;
		epoll_del(cs);
		close(cs);
		fds[cs].clean_fd();
	}
	else
	{
		buf[r] = '\0';
		fds[cs].buf_read += buf;

		while (true)
		{
			std::string cmd_line = client_read_line(cs);

			if (cmd_line.empty())
			{
				break;
			}

			std::cout << "Process Line from #" << cs << ": " << cmd_line << std::endl;

			handle_commands(cs, cmd_line);

			if (fds[cs].type != FD_CLIENT)
				continue;

			broadcast_message(cs, cmd_line);
		}
	}
}