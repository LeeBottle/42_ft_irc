#include "serverCommand/serverChannelCommand/CommandHelper.hpp"
#include "channel/Channel.hpp"
#include "client/Client.hpp"

#include <vector>

bool    CommandHelper::validChannel(
    const std::string &name)
{
    size_t index;

    if (name.size() < 2 || name[0] != '#')
        return (false);
    index = 0;
    while (index < name.size())
    {
        if (name[index] == ' ' || name[index] == ',' || name[index] == '\r'
            || name[index] == '\n' || name[index] == '\0')
            return (false);
        ++index;
    }
    return (true);
}

void    CommandHelper::reply(Client &client,
    const std::string &message)
{
    client.sendBuffer().append(message);
}

void    CommandHelper::namesReply(Client &client,
    Channel &channel)
{
    reply(client, ":ircserv 353 " + target(client)
        + " = " + channel.name() + " :" + memberNames(channel) + "\r\n");
    reply(client, ":ircserv 366 " + target(client)
        + " " + channel.name() + " :End of /NAMES list\r\n");
}

void    CommandHelper::topicReply(Client &client,
    Channel &channel)
{
    if (channel.modes().topic().empty())
        reply(client, ":ircserv 331 " + target(client)
            + " " + channel.name() + " :No topic is set\r\n");
    else
        reply(client, ":ircserv 332 " + target(client)
            + " " + channel.name() + " :" + channel.modes().topic()
            + "\r\n");
}

void    CommandHelper::toAll(Channel &channel,
    const std::string &message)
{
    std::vector<Client *>::const_iterator   it;
    const std::vector<Client *>             &members =
        channel.members().all();

    it = members.begin();
    while (it != members.end())
    {
        (*it)->sendBuffer().append(message);
        ++it;
    }
}

void    CommandHelper::toOthers(Channel &channel,
    Client &sender, const std::string &message)
{
    std::vector<Client *>::const_iterator   it;
    const std::vector<Client *>             &members =
        channel.members().all();

    it = members.begin();
    while (it != members.end())
    {
        if (*it != &sender)
            (*it)->sendBuffer().append(message);
        ++it;
    }
}

const std::string   &CommandHelper::target(
    Client &client)
{
    static const std::string unknownTarget = "*";

    if (client.hasNickname())
        return (client.nickname());
    return (unknownTarget);
}

std::string CommandHelper::memberNames(Channel &channel)
{
    std::vector<Client *>::const_iterator   it;
    const std::vector<Client *>             &members =
        channel.members().all();
    std::string                             names;

    it = members.begin();
    while (it != members.end())
    {
        if ((*it)->hasNickname())
        {
            if (!names.empty())
                names += " ";
            if (channel.operators().has(*it))
                names += "@";
            names += (*it)->nickname();
        }
        ++it;
    }
    return (names);
}
