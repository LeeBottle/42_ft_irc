#include "command/CommandModePrepare.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "parser/Parser.hpp"

#include <vector>

CommandModePrepare::CommandModePrepare(ChannelManager &channels,
    ClientManager &clients)
    : CommandBase(channels, clients)
{
}

CommandModePrepare::~CommandModePrepare()
{
}

bool    CommandModePrepare::executeUserMode(Client &client,
    const Parser &message)
{
    const std::vector<std::string>  &params = message.getParams();

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

bool    CommandModePrepare::shouldStop(Client &client,
    const Parser &message, Channel *&channel)
{
    const std::vector<std::string>  &params = message.getParams();

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
    if (executeUserMode(client, message))
        return (true);
    channel = _channels.findChannel(params[0]);
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
