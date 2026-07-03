#include "serverCommand/serverChannelCommand/command/Invite.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "parser/Parser.hpp"

#include <vector>

Invite::Invite(ClientManager &clients,
    ChannelManager &channels)
    : _clients(clients), _channels(channels)
{
}

Invite::~Invite()
{
}

bool    Invite::handle(Client &client,
    const Parser &message)
{
    const std::vector<std::string>  &params = message.params();
    Client                          *targetClient;
    Channel                         *channel;

    if (!client.isRegistered())
        CommandHelper::reply(client, ":ircserv 451 " + CommandHelper::target(client)
            + " :You have not registered\r\n");
    else if (params.size() < 2)
        CommandHelper::reply(client, ":ircserv 461 " + CommandHelper::target(client)
            + " INVITE :Not enough parameters\r\n");
    else
    {
        targetClient = _clients.findByNickname(params[0]);
        channel = _channels.find(params[1]);
        if (targetClient == NULL)
            CommandHelper::reply(client, ":ircserv 401 " + CommandHelper::target(client)
                + " " + params[0] + " :No such nick/channel\r\n");
        else if (channel == NULL)
            CommandHelper::reply(client, ":ircserv 403 " + CommandHelper::target(client)
                + " " + params[1] + " :No such channel\r\n");
        else if (!channel->members().has(&client))
            CommandHelper::reply(client, ":ircserv 442 " + CommandHelper::target(client)
                + " " + params[1] + " :You're not on that channel\r\n");
        else if (!channel->operators().has(&client))
            CommandHelper::reply(client, ":ircserv 482 " + CommandHelper::target(client)
                + " " + params[1] + " :You're not channel operator\r\n");
        else if (channel->members().has(targetClient))
            CommandHelper::reply(client, ":ircserv 443 " + CommandHelper::target(client)
                + " " + targetClient->nickname() + " " + params[1]
                + " :is already on channel\r\n");
        else
        {
            channel->invites().add(targetClient);
            CommandHelper::reply(client, ":ircserv 341 " + CommandHelper::target(client)
                + " " + targetClient->nickname() + " " + params[1]
                + "\r\n");
            targetClient->sendBuffer().append(":" + client.prefix()
                + " INVITE " + targetClient->nickname() + " :"
                + params[1] + "\r\n");
        }
    }
    return (true);
}
