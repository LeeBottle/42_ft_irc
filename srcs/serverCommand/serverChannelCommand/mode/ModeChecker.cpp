#include "serverCommand/serverChannelCommand/mode/ModeChecker.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "parser/Parser.hpp"

#include <vector>

ModeChecker::ModeChecker(ClientManager &clients,
    ChannelManager &channels)
    : _channels(channels)
{
    (void)clients;
}

ModeChecker::~ModeChecker()
{
}

bool    ModeChecker::prepare(Client &client,
    const Parser &message, Channel *&channel)
{
    const std::vector<std::string>  &params = message.params();

    if (!client.isRegistered())
        CommandHelper::reply(client, ":ircserv 451 " + CommandHelper::target(client)
            + " :You have not registered\r\n");
    else if (params.empty())
        CommandHelper::reply(client, ":ircserv 461 " + CommandHelper::target(client)
            + " MODE :Not enough parameters\r\n");
    else if (user(client, message))
        return (true);
    else if ((channel = _channels.find(params[0])) == NULL)
        CommandHelper::reply(client, ":ircserv 403 " + CommandHelper::target(client)
            + " " + params[0] + " :No such channel\r\n");
    else if (params.size() == 1)
        CommandHelper::reply(client, ":ircserv 324 " + CommandHelper::target(client)
            + " " + channel->name() + " " + channel->modes().modeString()
            + channel->modes().modeParameters() + "\r\n");
    else if (!channel->members().has(&client))
        CommandHelper::reply(client, ":ircserv 442 " + CommandHelper::target(client)
            + " " + params[0] + " :You're not on that channel\r\n");
    else if (!channel->operators().has(&client))
        CommandHelper::reply(client, ":ircserv 482 " + CommandHelper::target(client)
            + " " + params[0] + " :You're not channel operator\r\n");
    else
        return (false);
    return (true);
}

bool    ModeChecker::user(Client &client,
    const Parser &message)
{
    const std::vector<std::string>  &params = message.params();

    if (CommandHelper::validChannel(params[0]))
        return (false);
    if (params[0] != client.nickname())
    {
        CommandHelper::reply(client, ":ircserv 502 " + CommandHelper::target(client)
            + " :Cant change mode for other users\r\n");
        return (true);
    }
    CommandHelper::reply(client, ":ircserv 221 " + CommandHelper::target(client) + " +i\r\n");
    return (true);
}
