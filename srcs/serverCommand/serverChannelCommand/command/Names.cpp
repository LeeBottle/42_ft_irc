#include "serverCommand/serverChannelCommand/command/Names.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "parser/Parser.hpp"

#include <vector>

Names::Names(ClientManager &clients,
    ChannelManager &channels)
    : _channels(channels)
{
    (void)clients;
}

Names::~Names()
{
}

bool    Names::handle(Client &client,
    const Parser &message)
{
    const std::vector<std::string>  &params = message.params();
    Channel                         *channel;

    if (!client.isRegistered())
        CommandHelper::reply(client, ":ircserv 451 " + CommandHelper::target(client)
            + " :You have not registered\r\n");
    else if (params.empty())
        CommandHelper::reply(client, ":ircserv 461 " + CommandHelper::target(client)
            + " NAMES :Not enough parameters\r\n");
    else if (!CommandHelper::validChannel(params[0]))
        CommandHelper::reply(client, ":ircserv 403 " + CommandHelper::target(client)
            + " " + params[0] + " :No such channel\r\n");
    else
    {
        channel = _channels.find(params[0]);
        if (channel == NULL)
            CommandHelper::reply(client, ":ircserv 403 " + CommandHelper::target(client)
                + " " + params[0] + " :No such channel\r\n");
        else
            CommandHelper::namesReply(client, *channel);
    }
    return (true);
}
