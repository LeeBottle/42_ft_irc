#include "command/CommandChannelInfo.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "command/Command.hpp"

#include <vector>

CommandChannelInfo::CommandChannelInfo(ChannelManager &channels,
        ClientManager &clients)
    : CommandChannelBase(channels, clients)
{
}

CommandChannelInfo::~CommandChannelInfo()
{
}

void    CommandChannelInfo::handleNames(Client &client, const Command &command)
{
    const std::vector<std::string>  &params = command.getParams();
    Channel                         *channel;

    if (!client.isRegistered())
    {
        queueReply(client, ":ircserv 451 " + getReplyTarget(client)
            + " :You have not registered\r\n");
        return ;
    }
    if (params.empty())
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " NAMES :Not enough parameters\r\n");
        return ;
    }
    if (!isValidChannelName(params[0]))
    {
        queueReply(client, ":ircserv 403 " + getReplyTarget(client)
            + " " + params[0] + " :No such channel\r\n");
        return ;
    }
    channel = _channels.find(params[0]);
    if (channel == NULL)
    {
        queueReply(client, ":ircserv 403 " + getReplyTarget(client)
            + " " + params[0] + " :No such channel\r\n");
        return ;
    }
    sendNamesReply(client, *channel);
}

void    CommandChannelInfo::handleWho(Client &client, const Command &command)
{
    const std::vector<std::string>          &params = command.getParams();
    Channel                                 *channel;
    std::vector<Client *>::const_iterator   it;
    std::string                             realname;

    if (!client.isRegistered())
    {
        queueReply(client, ":ircserv 451 " + getReplyTarget(client)
            + " :You have not registered\r\n");
        return ;
    }
    if (params.empty())
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " WHO :Not enough parameters\r\n");
        return ;
    }
    channel = _channels.find(params[0]);
    if (channel != NULL)
    {
        const std::vector<Client *> &members = channel->getClients();

        it = members.begin();
        while (it != members.end())
        {
            realname = (*it)->getRealname();
            if (realname.empty())
                realname = (*it)->getNickname();
            queueReply(client, ":ircserv 352 " + getReplyTarget(client)
                + " " + channel->getName() + " " + (*it)->getUsername()
                + " localhost ircserv " + (*it)->getNickname()
                + " H :0 " + realname + "\r\n");
            ++it;
        }
    }
    queueReply(client, ":ircserv 315 " + getReplyTarget(client)
        + " " + params[0] + " :End of WHO list\r\n");
}

void    CommandChannelInfo::handleTopic(Client &client, const Command &command)
{
    const std::vector<std::string>  &params = command.getParams();
    Channel                         *channel;

    if (!client.isRegistered())
    {
        queueReply(client, ":ircserv 451 " + getReplyTarget(client)
            + " :You have not registered\r\n");
        return ;
    }
    if (params.empty())
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " TOPIC :Not enough parameters\r\n");
        return ;
    }
    channel = _channels.find(params[0]);
    if (channel == NULL)
    {
        queueReply(client, ":ircserv 403 " + getReplyTarget(client)
            + " " + params[0] + " :No such channel\r\n");
        return ;
    }
    if (params.size() == 1)
    {
        sendTopicReply(client, *channel);
        return ;
    }
    if (!channel->hasClient(&client))
    {
        queueReply(client, ":ircserv 442 " + getReplyTarget(client)
            + " " + params[0] + " :You're not on that channel\r\n");
        return ;
    }
    if (!channel->canSetTopic(&client))
    {
        queueReply(client, ":ircserv 482 " + getReplyTarget(client)
            + " " + params[0] + " :You're not channel operator\r\n");
        return ;
    }
    channel->modes().setTopic(params[1]);
    channel->broadcastAll(":" + client.getPrefix() + " TOPIC "
        + channel->getName() + " :" + params[1] + "\r\n");
}
