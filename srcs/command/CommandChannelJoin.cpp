#include "command/CommandChannelJoin.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "command/Command.hpp"

#include <vector>

CommandChannelJoin::CommandChannelJoin(ChannelManager &channels,
    ClientManager &clients)
    : CommandChannelBase(channels, clients)
{
}

CommandChannelJoin::~CommandChannelJoin()
{
}

void    CommandChannelJoin::handleJoin(Client &client, const Command &command)
{
    const std::vector<std::string>  &params = command.getParams();
    Channel                         *channel;
    Channel::JoinResult             result;
    std::string                     key;

    if (!client.isRegistered())
    {
        queueReply(client, ":ircserv 451 " + getReplyTarget(client)
            + " :You have not registered\r\n");
        return ;
    }
    if (params.empty())
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " JOIN :Not enough parameters\r\n");
        return ;
    }
    if (!isValidChannelName(params[0]))
    {
        queueReply(client, ":ircserv 403 " + getReplyTarget(client)
            + " " + params[0] + " :No such channel\r\n");
        return ;
    }
    if (params.size() > 1)
        key = params[1];
    channel = _channels.getOrCreate(params[0]);
    result = channel->canJoin(&client, key);
    if (result == Channel::JOIN_INVITE_ONLY)
    {
        queueReply(client, ":ircserv 473 " + getReplyTarget(client)
            + " " + params[0] + " :Cannot join channel (+i)\r\n");
        return ;
    }
    if (result == Channel::JOIN_BAD_KEY)
    {
        queueReply(client, ":ircserv 475 " + getReplyTarget(client)
            + " " + params[0] + " :Cannot join channel (+k)\r\n");
        return ;
    }
    if (result == Channel::JOIN_FULL)
    {
        queueReply(client, ":ircserv 471 " + getReplyTarget(client)
            + " " + params[0] + " :Cannot join channel (+l)\r\n");
        return ;
    }
    channel->addClient(&client);
    channel->broadcastAll(":" + client.getPrefix() + " JOIN "
        + channel->getName() + "\r\n");
    if (!channel->modes().getTopic().empty())
        sendTopicReply(client, *channel);
    sendNamesReply(client, *channel);
}

void    CommandChannelJoin::handlePart(Client &client, const Command &command)
{
    const std::vector<std::string>  &params = command.getParams();
    Channel                         *channel;
    std::string                     message;

    if (!client.isRegistered())
    {
        queueReply(client, ":ircserv 451 " + getReplyTarget(client)
            + " :You have not registered\r\n");
        return ;
    }
    if (params.empty())
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " PART :Not enough parameters\r\n");
        return ;
    }
    channel = _channels.find(params[0]);
    if (channel == NULL)
    {
        queueReply(client, ":ircserv 403 " + getReplyTarget(client)
            + " " + params[0] + " :No such channel\r\n");
        return ;
    }
    if (!channel->hasClient(&client))
    {
        queueReply(client, ":ircserv 442 " + getReplyTarget(client)
            + " " + params[0] + " :You're not on that channel\r\n");
        return ;
    }
    message = "Leaving";
    if (params.size() > 1 && !params[1].empty())
        message = params[1];
    channel->broadcastAll(":" + client.getPrefix() + " PART "
        + channel->getName() + " :" + message + "\r\n");
    channel->removeClient(&client);
    _channels.deleteIfEmpty(channel);
}
