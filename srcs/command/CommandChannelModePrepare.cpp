#include "command/CommandChannelModePrepare.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "command/Command.hpp"

#include <vector>

CommandChannelModePrepare::CommandChannelModePrepare(ChannelManager &channels,
    ClientManager &clients)
    : CommandChannelBase(channels, clients)
{
}

CommandChannelModePrepare::~CommandChannelModePrepare()
{
}

bool    CommandChannelModePrepare::handleUserMode(Client &client,
    const Command &command)
{
    const std::vector<std::string>  &params = command.getParams();

    if (isValidChannelName(params[0]))
        return (false);
    if (params[0] != client.getNickname())
    {
        queueReply(client, ":ircserv 502 " + getReplyTarget(client)
            + " :Cant change mode for other users\r\n");
        return (true);
    }
    queueReply(client, ":ircserv 221 " + getReplyTarget(client)
        + " +i\r\n");
    return (true);
}

bool    CommandChannelModePrepare::shouldStop(Client &client,
    const Command &command, Channel *&channel)
{
    const std::vector<std::string>  &params = command.getParams();

    if (!client.isRegistered())
    {
        queueReply(client, ":ircserv 451 " + getReplyTarget(client)
            + " :You have not registered\r\n");
        return (true);
    }
    if (params.empty())
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " MODE :Not enough parameters\r\n");
        return (true);
    }
    if (handleUserMode(client, command))
        return (true);
    channel = _channels.find(params[0]);
    if (channel == NULL)
    {
        queueReply(client, ":ircserv 403 " + getReplyTarget(client)
            + " " + params[0] + " :No such channel\r\n");
        return (true);
    }
    if (params.size() == 1)
    {
        queueReply(client, ":ircserv 324 " + getReplyTarget(client)
            + " " + channel->getName() + " " + channel->modes().getModeString()
            + channel->modes().getModeParameters() + "\r\n");
        return (true);
    }
    if (!channel->hasClient(&client))
    {
        queueReply(client, ":ircserv 442 " + getReplyTarget(client)
            + " " + params[0] + " :You're not on that channel\r\n");
        return (true);
    }
    if (!channel->isOperator(&client))
    {
        queueReply(client, ":ircserv 482 " + getReplyTarget(client)
            + " " + params[0] + " :You're not channel operator\r\n");
        return (true);
    }
    return (false);
}
