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
	else if (cmd_line.find("NICK") == 0)
	{
		handle_command_nick(cs, cmd_line);
		return;
	}
	else if (cmd_line.find("USER") == 0)
	{
		handle_command_user(cs, cmd_line);
		return;
	}
	else if (cmd_line.find("PRIVMSG") == 0)
	{
		handle_command_privmsg(cs, cmd_line);
		return;
	}

	if (fds[cs].type != FD_CLIENT || fds[cs].nickname.empty() || fds[cs].username.empty())
	{
		std::cout << "Security Alert: Unauthenticated Client #" << cs << " tried command: " << cmd_line << std::endl;
		
		std::string err_msg = "You have not registered\r\n";
		fds[cs].buf_write += err_msg;
		epoll_mod(cs, EPOLLIN | EPOLLOUT);
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
	if (fds[cs].type == FD_CLIENT)
	{
		std::cout << "Client #" << cs << " tried to send PASS again. (Ignored)" << std::endl;
		fds[cs].buf_write += "Already registered password\r\n";
		epoll_mod(cs, EPOLLIN | EPOLLOUT);
		return;
	}
	if (cmd_line.size() <= 4 || (cmd_line.size() > 4 && !std::isspace(static_cast<unsigned char>(cmd_line[4]))))
	{
		fds[cs].buf_write += ":ircserv 461 * PASS :Not enough parameters\r\n";
		epoll_mod(cs, EPOLLIN | EPOLLOUT);
		return;
	}

	std::string input_pw = cmd_line.substr(5);
	if (input_pw.empty())
	{
		fds[cs].buf_write += ":ircserv 461 * PASS :Not enough parameters\r\n";
		epoll_mod(cs, EPOLLIN | EPOLLOUT);
		return;
	}

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

void env::handle_command_nick(int cs, const std::string& cmd_line)
{
	if (cmd_line.size() <= 4)
		return;

	size_t start = 4;
	while (start < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[start])))
		start++;
	if (start >= cmd_line.length())
		return;

	size_t end = start;
	while (end < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[end])))
		end++;

	std::string new_nick = cmd_line.substr(start, end - start);
	if (new_nick.length() > 9)
		new_nick = new_nick.substr(0, 9);

	for (size_t i = 0; i < fds.size(); ++i)
	{
		if (fds[i].type == FD_CLIENT && static_cast<int>(i) != cs && fds[i].nickname == new_nick)
		{
			std::cout << "Conflict: Nickname '" << new_nick << "' is already in use by FD #" << i << std::endl;

			std::string err_msg = ":ircserv 433 " + new_nick + " :Nickname is already in use.\r\n";
			fds[cs].buf_write += err_msg;
			epoll_mod(cs, EPOLLIN | EPOLLOUT);
			return;
		}
	}
	fds[cs].nickname = new_nick;
	std::cout << "Client #" << cs << " successfully set nickname to: " << fds[cs].nickname << std::endl;

	if (!fds[cs].nickname.empty() && !fds[cs].username.empty() && fds[cs].type == FD_CLIENT)
	{
		std::string welcome_msg = ":ircserv 001 " + fds[cs].nickname + " :Welcome to the Internet Relay Network " + fds[cs].nickname + "\r\n";
		fds[cs].buf_write += welcome_msg;
		epoll_mod(cs, EPOLLIN | EPOLLOUT);
	}
}

void env::handle_command_user(int cs, const std::string& cmd_line)
{
	if (cmd_line.size() <= 4)
		return;

	size_t start = 4;
	while (start < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[start])))
		start++;
	if (start >= cmd_line.length())
		return;

	size_t end = start;
	while (end < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[end])))
		end++;

	std::string parsed_user = cmd_line.substr(start, end - start);
	if (parsed_user.length() > 9)
		parsed_user = parsed_user.substr(0, 9);
	fds[cs].username = parsed_user;

	if (!fds[cs].nickname.empty() && !fds[cs].username.empty() && fds[cs].type == FD_CLIENT)
	{
		std::string welcome_msg = ":ircserv 001 " + fds[cs].nickname + " :Welcome to the Internet Relay Network " + fds[cs].nickname + "\r\n";
		
		fds[cs].buf_write += welcome_msg;
		epoll_mod(cs, EPOLLIN | EPOLLOUT);
	}
}

void env::handle_command_privmsg(int cs, const std::string& cmd_line)
{
	if (fds[cs].type != FD_CLIENT)
	{
		fds[cs].buf_write += ":ircserv 451 * :You must specify a password first (PASS).\r\n";
		epoll_mod(cs, EPOLLIN | EPOLLOUT);
		return;
	}

	std::string pure_message;

	size_t colon_pos = cmd_line.find(':');
	
	if (colon_pos != std::string::npos)
	{
		pure_message = cmd_line.substr(colon_pos + 1);
	}
	else
	{
		size_t space_pos = cmd_line.find(' ', 8);
		if (space_pos != std::string::npos)
			pure_message = cmd_line.substr(space_pos + 1);
		else if (cmd_line.size() > 8)
			pure_message = cmd_line.substr(8);
		else
		{
			fds[cs].buf_write += ":ircserv 412 " + fds[cs].nickname + " :No text to send\r\n";
			epoll_mod(cs, EPOLLIN | EPOLLOUT);
			return;
		}
	}

	broadcast_message(cs, pure_message);
}
