#include "command/CommandInviteKick.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "parser/Parser.hpp"

#include <vector>

CommandInviteKick::CommandInviteKick(ChannelManager &channels,
    ClientManager &clients)
    : CommandBase(channels, clients)
{
}

CommandInviteKick::~CommandInviteKick()
{
}

void    CommandInviteKick::executeInvite(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.getParams();
    Client                          *targetClient;
    Channel                         *channel;

    if (!client.isRegistered())
    {
        queueReply(client, ":ircserv 451 " + getReplyTarget(client)
            + " :You have not registered\r\n");
        return ;
    }
    if (params.size() < 2)
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " INVITE :Not enough parameters\r\n");
        return ;
    }
    targetClient = _clients.findByNickname(params[0]);
    if (targetClient == NULL)
    {
        queueReply(client, ":ircserv 401 " + getReplyTarget(client)
            + " " + params[0] + " :No such nick/channel\r\n");
        return ;
    }
    channel = _channels.findChannel(params[1]);
    if (channel == NULL)
    {
        queueReply(client, ":ircserv 403 " + getReplyTarget(client)
            + " " + params[1] + " :No such channel\r\n");
        return ;
    }
    if (!channel->hasClient(&client))
    {
        queueReply(client, ":ircserv 442 " + getReplyTarget(client)
            + " " + params[1] + " :You're not on that channel\r\n");
        return ;
    }
    if (!channel->isOperator(&client))
    {
        queueReply(client, ":ircserv 482 " + getReplyTarget(client)
            + " " + params[1] + " :You're not channel operator\r\n");
        return ;
    }
    if (channel->hasClient(targetClient))
    {
        queueReply(client, ":ircserv 443 " + getReplyTarget(client)
            + " " + targetClient->getNickname() + " " + params[1]
            + " :is already on channel\r\n");
        return ;
    }
    channel->inviteClient(targetClient);
    queueReply(client, ":ircserv 341 " + getReplyTarget(client)
        + " " + targetClient->getNickname() + " " + params[1] + "\r\n");
    targetClient->queueSend(":" + client.getPrefix() + " INVITE "
        + targetClient->getNickname() + " :" + params[1] + "\r\n");
}

void    CommandInviteKick::executeKick(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.getParams();
    Channel                         *channel;
    Client                          *targetClient;
    std::string                     comment;

    if (!client.isRegistered())
    {
        queueReply(client, ":ircserv 451 " + getReplyTarget(client)
            + " :You have not registered\r\n");
        return ;
    }
    if (params.size() < 2)
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " KICK :Not enough parameters\r\n");
        return ;
    }
    channel = _channels.findChannel(params[0]);
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
    if (!channel->isOperator(&client))
    {
        queueReply(client, ":ircserv 482 " + getReplyTarget(client)
            + " " + params[0] + " :You're not channel operator\r\n");
        return ;
    }
    targetClient = _clients.findByNickname(params[1]);
    if (targetClient == NULL || !channel->hasClient(targetClient))
    {
        queueReply(client, ":ircserv 441 " + getReplyTarget(client)
            + " " + params[1] + " " + params[0]
            + " :They aren't on that channel\r\n");
        return ;
    }
    comment = client.getNickname();
    if (params.size() > 2 && !params[2].empty())
        comment = params[2];
    channel->broadcastAll(":" + client.getPrefix() + " KICK "
        + channel->getName() + " " + targetClient->getNickname()
        + " :" + comment + "\r\n");
    channel->removeClient(targetClient);
    _channels.removeEmptyChannel(channel);
}
