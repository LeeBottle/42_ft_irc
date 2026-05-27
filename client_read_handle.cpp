#include "bircd.hpp"

void env::handle_commands(int cs, const std::string& cmd_line)
{
	if (cmd_line.find("CAP") == 0)
	{
		return;
	}
	else if (cmd_line.find("PASS") == 0)
	{
		handle_command_pass(cs, cmd_line);
		return;
	}
	else if (fds[cs].type != FD_CLIENT)
	{
		std::cout << "Client #" << cs << " rejected (Tried to chat without PASS)." << std::endl;
		send(cs, ":ircserv 451 * :You must specify a password first (PASS).\r\n", 58, 0);

		epoll_del(cs);
		close(cs);
		fds[cs].clean_fd();
		return;
	}
}

void env::handle_command_pass(int cs, const std::string& cmd_line)
{
	std::string input_pw = cmd_line.substr(5);

	if (input_pw == password)
	{
		fds[cs].type = FD_CLIENT;

		std::cout << "Client #" << cs << " successfully authorized." << std::endl;

		fds[cs].buf_write += "Password authorized\r\n";
		epoll_mod(cs, EPOLLIN | EPOLLOUT);
	}
	else
	{
		std::cout << "Client #" << cs << " kicked (Incorrect password)." << std::endl;

		fds[cs].buf_write += "Password incorrect.\r\n";
		epoll_mod(cs, EPOLLIN | EPOLLOUT);
		
		epoll_del(cs);
		close(cs);
		fds[cs].clean_fd();
	}
}