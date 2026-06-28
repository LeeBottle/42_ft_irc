#include "command/CommandChannelMessage.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "command/Command.hpp"

#include <vector>

CommandChannelMessage::CommandChannelMessage(ChannelManager &channels,
    ClientManager &clients)
    : CommandChannelBase(channels, clients)
{
}

CommandChannelMessage::~CommandChannelMessage()
{
}

void    CommandChannelMessage::handlePrivmsg(Client &client,
    const Command &command)
{
    const std::vector<std::string>  &params = command.getParams();

    if (!client.isRegistered())
    {
        queueReply(client, ":ircserv 451 " + getReplyTarget(client)
            + " :You have not registered\r\n");
        return ;
    }
    if (params.empty())
    {
        queueReply(client, ":ircserv 411 " + getReplyTarget(client)
            + " :No recipient given (PRIVMSG)\r\n");
        return ;
    }
    if (params.size() < 2 || params[1].empty())
    {
        queueReply(client, ":ircserv 412 " + getReplyTarget(client)
            + " :No text to send\r\n");
        return ;
    }
    if (!isValidChannelName(params[0]))
    {
        Client *targetClient;

        targetClient = _clients.findByNickname(params[0]);
        if (targetClient == NULL)
        {
            queueReply(client, ":ircserv 401 " + getReplyTarget(client)
                + " " + params[0] + " :No such nick/channel\r\n");
            return ;
        }
        targetClient->queueSend(":" + client.getPrefix()
            + " PRIVMSG " + targetClient->getNickname() + " :"
            + params[1] + "\r\n");
        return ;
    }
    sendChannelMessage(client, params[0], params[1]);
}

void    CommandChannelMessage::sendChannelMessage(Client &client,
    const std::string &target, const std::string &text)
{
    Channel *channel;

    channel = _channels.find(target);
    if (channel == NULL)
    {
        queueReply(client, ":ircserv 403 " + getReplyTarget(client)
            + " " + target + " :No such channel\r\n");
        return ;
    }
    if (!channel->hasClient(&client))
    {
        queueReply(client, ":ircserv 404 " + getReplyTarget(client)
            + " " + target + " :Cannot send to channel\r\n");
        return ;
    }
    channel->broadcast(client, ":" + client.getPrefix()
        + " PRIVMSG " + target + " :" + text + "\r\n");
}
