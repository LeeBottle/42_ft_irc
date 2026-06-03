#include "bircd.hpp"

void env::handle_commands(int cs, const std::string& cmd_line)
{
	if (cmd_line.find("CAP") == 0)
	{
		if (fds[cs].type != FD_CLIENT)
		{
			if (fds[cs].buf_read.find('\n') != std::string::npos)
			{
				if (fds[cs].buf_read.find("PASS") == std::string::npos)
				{
					std::cout << "Client #" << cs << " irssi connect failed: Missing PASS command." << std::endl;
					fds[cs].buf_write += ":ircserv 461 * PASS :Not enough parameters\r\n";
					fds[cs].close_after_write = true;
					return;
				}
			}
		}
		return;
	}

	if (fds[cs].type != FD_CLIENT)
	{
		if (cmd_line.find("PASS") == 0)
		{
			handle_command_pass(cs, cmd_line);
			return;
		}
		else
		{
			std::cout << "Security Alert: Unauthenticated Client #" << cs << " bypassed PASS. Command ignored: " << cmd_line << std::endl;
			fds[cs].buf_write += ":ircserv 451 * :You have not registered\r\n";
			fds[cs].close_after_write = true;
			return;
		}
	}

	if (cmd_line.find("PASS") == 0)
	{
		std::cout << "Client #" << cs << " tried to send PASS again after authorization." << std::endl;
		fds[cs].buf_write += ":ircserv 462 * :Unauthorized command (Already registered)\r\n";
		return;
	}
	else if (cmd_line.find("NICK") == 0)
	{
		handle_command_nick(cs, cmd_line);
	}
	else if (cmd_line.find("USER") == 0)
	{
		handle_command_user(cs, cmd_line);
	}
	else if (cmd_line.find("PRIVMSG") == 0)
	{
		handle_command_privmsg(cs, cmd_line);
	}
	else if (cmd_line.find("JOIN") == 0)
	{
		handle_command_join(cs, cmd_line);
	}
	else if (cmd_line.find("PART") == 0)
	{
		handle_command_part(cs, cmd_line);
	}
}

void env::handle_command_pass(int cs, const std::string& cmd_line)
{
	// 이미 성공적으로 가입된 상태에서 PASS 중복 시도 시 에러 처리
	if (fds[cs].type == FD_CLIENT && !fds[cs].nickname.empty())
	{
		std::cout << "Client #" << cs << " tried to send PASS again. Sending 462 Already registered." << std::endl;
		fds[cs].buf_write += ":ircserv 462 " + fds[cs].nickname + " :You may not reregister\r\n";
		return;
	}

	size_t idx = 4;
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	// 인자가 누락된 경우 (461 ERR_NEEDMOREPARAMS)
	if (idx >= cmd_line.length())
	{
		std::cout << "Client #" << cs << " auth failed: Not enough parameters." << std::endl;
		fds[cs].buf_write += ":ircserv 461 * PASS :Not enough parameters\r\n";
		fds[cs].close_after_write = true;
		return;
	}

	std::string input_pw = cmd_line.substr(idx);
	while (!input_pw.empty() && std::isspace(static_cast<unsigned char>(input_pw[input_pw.size() - 1])))
	{
		input_pw.erase(input_pw.size() - 1);
	}

	if (input_pw.empty())
	{
		fds[cs].buf_write += ":ircserv 461 * PASS :Not enough parameters\r\n";
		fds[cs].close_after_write = true;
		return;
	}

	// 비밀번호 매칭 검증
	if (input_pw == password)
	{
		fds[cs].type = FD_CLIENT; // 다음 명령어(NICK, USER)를 받아들이기 위해 식별자 변경
		std::cout << "Client #" << cs << " password successfully authorized." << std::endl;
	}
	else
	{
		// 비밀번호가 틀린 경우 (464 ERR_PASSWDMISMATCH)
		std::cout << "Client #" << cs << " auth failed: Incorrect password." << std::endl;
		fds[cs].buf_write += ":ircserv 464 * :Password incorrect\r\n";
		fds[cs].close_after_write = true;
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


	bool is_valid = true;
	if (new_nick.empty())
	{
		is_valid = false;
	}
	else
	{
		if (new_nick[0] >= '0' && new_nick[0] <= '9')
		{
			is_valid = false;
		}
		for (size_t i = 0; i < new_nick.length(); ++i)
		{
			char ch = new_nick[i];
			if (ch == ',' || ch == '*' || ch == '?' || ch == '!' || ch == '@' || ch == '.')
			{
				is_valid = false;
			}
		}
	}


	for (size_t i = 0; i < fds.size(); ++i)
	{
		if (fds[i].type == FD_CLIENT && static_cast<int>(i) != cs && fds[i].nickname == new_nick)
		{
			std::cout << "Conflict: Nickname '" << new_nick << "' is already in use by FD #" << i << std::endl;
			fds[cs].buf_write += ":ircserv 433 " + new_nick + " :Nickname is already in use.\r\n";
			return;
		}
	}


	if (fds[cs].is_welcomed)
	{
		std::string nick_change_msg = ":" + fds[cs].nickname + "!" + fds[cs].username + "@127.0.0.1 NICK :" + new_nick + "\r\n";
		fds[cs].buf_write += nick_change_msg;
		
		for (size_t i = 0; i < fds.size(); ++i)
		{
			if (fds[i].type == FD_CLIENT && static_cast<int>(i) != cs)
			{
				fds[i].buf_write += nick_change_msg;
			}
		}
	}

	fds[cs].nickname = new_nick;
	std::cout << "Client #" << cs << " set nickname to: " << fds[cs].nickname << std::endl;

	if (!fds[cs].nickname.empty() && !fds[cs].username.empty() && fds[cs].is_welcomed == false)
	{
		fds[cs].is_welcomed = true;
		fds[cs].buf_write += ":ircserv 001 " + fds[cs].nickname + " :Welcome to the Internet Relay Network " + fds[cs].nickname + "\r\n";
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
	std::cout << "Client #" << cs << " set username to: " << fds[cs].username << std::endl;

	if (!fds[cs].nickname.empty() && !fds[cs].username.empty() && fds[cs].is_welcomed == false)
	{
		fds[cs].is_welcomed = true;
		fds[cs].buf_write += ":ircserv 001 " + fds[cs].nickname + " :Welcome to the Internet Relay Network " + fds[cs].nickname + "\r\n";
	}
}

void env::handle_command_privmsg(int cs, const std::string& cmd_line)
{
	if (fds[cs].type != FD_CLIENT)
	{
		fds[cs].buf_write += ":ircserv 451 * :You must specify a password first (PASS).\r\n";
		return;
	}


	size_t idx = 7;
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 411 " + (fds[cs].nickname.empty() ? "*" : fds[cs].nickname) + " :No recipient given (PRIVMSG)\r\n";
		return;
	}


	size_t target_start = idx;
	while (idx < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[idx])) && cmd_line[idx] != ':')
	{
		idx++;
	}
	std::string target = cmd_line.substr(target_start, idx - target_start);



	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 412 " + fds[cs].nickname + " :No text to send\r\n";
		return;
	}


	std::string pure_message;
	if (cmd_line[idx] == ':')
	{
		pure_message = cmd_line.substr(idx + 1);
	}
	else
	{
		pure_message = cmd_line.substr(idx);
	}

	broadcast_message(cs, target, pure_message);
}

void env::handle_command_join(int cs, const std::string& cmd_line)
{
	size_t idx = 4;
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 461 " + fds[cs].nickname + " JOIN :Not enough parameters\r\n";
		return;
	}

	size_t ch_start = idx;
	while (idx < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}
	std::string ch_name = cmd_line.substr(ch_start, idx - ch_start);

	if (ch_name.empty())
	{
		return;
	}
	if (ch_name[0] != '#' && ch_name[0] != '&')
	{
		fds[cs].buf_write += ":ircserv 403 " + fds[cs].nickname + " " + ch_name + " :No such channel\r\n";
		return;
	}

	std::map<std::string, Channel>::iterator it = channels.find(ch_name);
	
	if (it == channels.end())
	{
		std::cout << "Creating new channel: " << ch_name << " by Client #" << cs << std::endl;
		
		Channel new_channel(ch_name);
		new_channel.add_client(cs);
		new_channel.add_operator(cs);
		
		channels.insert(std::make_pair(ch_name, new_channel));
	}
	else
	{
		std::cout << "Client #" << cs << " joining existing channel: " << ch_name << std::endl;
		it->second.add_client(cs);
	}

	std::string join_notif = ":" + fds[cs].nickname + "!" + fds[cs].username + "@127.0.0.1 JOIN :" + ch_name + "\r\n";
	fds[cs].buf_write += join_notif;

	std::vector<int> members = channels[ch_name].get_client_fds();
	for (size_t i = 0; i < members.size(); ++i)
	{
		int target_fd = members[i];
		if (target_fd != cs)
		{
			fds[target_fd].buf_write += join_notif;
		}
	}

	std::string current_topic = channels[ch_name].get_topic();
	if (current_topic.empty())
	{
		fds[cs].buf_write += ":ircserv 331 " + fds[cs].nickname + " " + ch_name + " :No topic is set\r\n";
	}
	else
	{
		fds[cs].buf_write += ":ircserv 332 " + fds[cs].nickname + " " + ch_name + " :" + current_topic + "\r\n";
	}
}

void env::handle_command_part(int cs, const std::string& cmd_line)
{
	size_t idx = 4;
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 461 " + fds[cs].nickname + " PART :Not enough parameters\r\n";
		return;
	}

	size_t ch_start = idx;
	while (idx < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[idx])) && cmd_line[idx] != ':')
	{
		idx++;
	}
	std::string ch_name = cmd_line.substr(ch_start, idx - ch_start);

	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	std::string reason = "";
	if (idx < cmd_line.length())
	{
		if (cmd_line[idx] == ':')
		{
			reason = cmd_line.substr(idx + 1);
		}
		else
		{
			reason = cmd_line.substr(idx);
		}
	}

	std::map<std::string, Channel>::iterator it = channels.find(ch_name);
	if (it == channels.end())
	{
		fds[cs].buf_write += ":ircserv 403 " + fds[cs].nickname + " " + ch_name + " :No such channel\r\n";
		return;
	}

	Channel& channel = it->second;

	if (channel.is_member(cs) == false)
	{
		fds[cs].buf_write += ":ircserv 442 " + fds[cs].nickname + " " + ch_name + " :You're not on that channel\r\n";
		return;
	}

	std::string part_packet = ":" + fds[cs].nickname + "!" + fds[cs].username + "@127.0.0.1 PART " + ch_name;
	if (reason.empty() == false)
	{
		part_packet += " :" + reason;
	}
	part_packet += "\r\n";

	std::vector<int> members = channel.get_client_fds();
	for (size_t i = 0; i < members.size(); ++i)
	{
		int target_fd = members[i];
		fds[target_fd].buf_write += part_packet;
	}

	channel.remove_client(cs);
	std::cout << "Client #" << cs << " left channel: " << ch_name << std::endl;

	std::vector<int> remaining_members = channel.get_client_fds();
	if (remaining_members.empty() == true)
	{
		std::cout << "Channel " << ch_name << " is now empty. Dissolving channel resource." << std::endl;
		channels.erase(it);
	}
}