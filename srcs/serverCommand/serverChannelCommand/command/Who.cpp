#include "serverCommand/serverChannelCommand/command/Who.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "parser/Parser.hpp"

#include <vector>

Who::Who(ClientManager &clients,
    ChannelManager &channels)
    : _channels(channels)
{
    (void)clients;
}

Who::~Who()
{
}

bool    Who::handle(Client &client,
    const Parser &message)
{
    const std::vector<std::string>          &params = message.params();
    Channel                                 *channel;
    std::vector<Client *>::const_iterator   it;
    std::string                             realname;

    if (!client.isRegistered())
        CommandHelper::reply(client, ":ircserv 451 " + CommandHelper::target(client)
            + " :You have not registered\r\n");
    else if (params.empty())
        CommandHelper::reply(client, ":ircserv 461 " + CommandHelper::target(client)
            + " WHO :Not enough parameters\r\n");
    else
    {
        channel = _channels.find(params[0]);
        if (channel != NULL)
        {
            const std::vector<Client *> &members =
                channel->members().all();

            it = members.begin();
            while (it != members.end())
            {
                realname = (*it)->realname();
                if (realname.empty())
                    realname = (*it)->nickname();
                CommandHelper::reply(client, ":ircserv 352 " + CommandHelper::target(client)
                    + " " + channel->name() + " " + (*it)->username()
                    + " localhost ircserv " + (*it)->nickname()
                    + " H :0 " + realname + "\r\n");
                ++it;
            }
        }
        CommandHelper::reply(client, ":ircserv 315 " + CommandHelper::target(client)
            + " " + params[0] + " :End of WHO list\r\n");
    }
    return (true);
}
