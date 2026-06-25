#include "bircd.hpp"

void	env::broadcast_message(int sender_cs, const std::string& target, const std::string& message)
{
	std::string sender_nick = fds[sender_cs].nickname;
	std::string sender_user = fds[sender_cs].username;
	std::string sender_host = "127.0.0.1";

	if (sender_nick.empty())
	{
		sender_nick = "unknown";
	}
	if (sender_user.empty())
	{
		sender_user = "unknown";
	}

	std::string formatted_packet = ":" + sender_nick + "!" + sender_user + "@" + sender_host  + " PRIVMSG " + target + " :" + message + "\r\n";

	if (target.empty() == false && (target[0] == '#' || target[0] == '&'))
	{
		std::map<std::string, Channel>::iterator it = channels.find(target);
		
		if (it == channels.end())
		{
			fds[sender_cs].buf_write += ":ircserv 401 " + sender_nick + " " + target + " :No such channel\r\n";
			return;
		}

		Channel& channel = it->second;

		if (channel.is_member(sender_cs) == false)
		{
			fds[sender_cs].buf_write += ":ircserv 442 " + sender_nick + " " + target + " :You're not on that channel\r\n";
			return;
		}

		std::vector<int> members = channel.get_client_fds();
		for (size_t i = 0; i < members.size(); ++i)
		{
			int member_fd = members[i];
			if (member_fd != sender_cs)
			{
				fds[member_fd].buf_write += formatted_packet;
			}
		}
	}
	else
	{
		if (target == sender_nick)
		{
			return;
		}

		bool user_found = false;
		for (size_t i = 0; i < fds.size(); ++i)
		{
			// 2. static_cast<int>(i) != sender_cs 조건을 추가하여 한 번 더 안전장치를 둡니다.
			if (fds[i].type == FD_CLIENT && fds[i].nickname == target && static_cast<int>(i) != sender_cs)
			{
				fds[i].buf_write += formatted_packet;
				user_found = true;
				break;
			}
		}

		if (user_found == false)
		{
			fds[sender_cs].buf_write += ":ircserv 401 " + sender_nick + " " + target + " :No such nick\r\n";
		}
	}
}