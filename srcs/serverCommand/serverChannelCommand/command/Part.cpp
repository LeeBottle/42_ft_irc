#include "serverCommand/serverChannelCommand/command/Part.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "parser/Parser.hpp"

#include <vector>

Part::Part(ClientManager &clients,
    ChannelManager &channels)
    : _channels(channels)
{
    (void)clients;
}

Part::~Part()
{
}

bool    Part::handle(Client &client,
    const Parser &message)
{
    const std::vector<std::string>  &params = message.params();
    Channel                         *channel;
    std::string                     partMessage;

    if (!client.isRegistered())
    {
        CommandHelper::reply(client, ":ircserv 451 "
            + CommandHelper::target(client)
            + " :You have not registered\r\n");
        return (true);
    }
    if (params.empty())
    {
        CommandHelper::reply(client, ":ircserv 461 "
            + CommandHelper::target(client)
            + " PART :Not enough parameters\r\n");
        return (true);
    }
    channel = _channels.find(params[0]);
    if (channel == NULL)
        CommandHelper::reply(client, ":ircserv 403 " + CommandHelper::target(client)
            + " " + params[0] + " :No such channel\r\n");
    else if (!channel->members().has(&client))
        CommandHelper::reply(client, ":ircserv 442 " + CommandHelper::target(client)
            + " " + params[0] + " :You're not on that channel\r\n");
    else
    {
        partMessage = "Leaving";
        if (params.size() > 1 && !params[1].empty())
            partMessage = params[1];
        CommandHelper::toAll(*channel, ":" + client.prefix() + " PART "
            + channel->name() + " :" + partMessage + "\r\n");
        channel->members().remove(&client);
        channel->operators().remove(&client);
        channel->invites().remove(&client);
        _channels.removeEmpty(channel);
    }
    return (true);
}
