#include "command/CommandBase.hpp"
#include "channel/Channel.hpp"
#include "client/Client.hpp"

CommandBase::CommandBase(ChannelManager &channels, ClientManager &clients)
    : _channels(channels), _clients(clients)
{
}

CommandBase::~CommandBase()
{
}

bool    CommandBase::isValidChannelName(const std::string &name) const
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

void    CommandBase::sendNamesReply(Client &client, Channel &channel)
{
    queueReply(client, ":ircserv 353 " + getReplyTarget(client)
        + " = " + channel.getName() + " :" + channel.getMemberNames() + "\r\n");
    queueReply(client, ":ircserv 366 " + getReplyTarget(client)
        + " " + channel.getName() + " :End of /NAMES list\r\n");
}

void    CommandBase::sendTopicReply(Client &client, Channel &channel)
{
    if (channel.modes().getTopic().empty())
        queueReply(client, ":ircserv 331 " + getReplyTarget(client)
            + " " + channel.getName() + " :No topic is set\r\n");
    else
        queueReply(client, ":ircserv 332 " + getReplyTarget(client)
            + " " + channel.getName() + " :" + channel.modes().getTopic() + "\r\n");
}

void    CommandBase::queueReply(Client &client, const std::string &message)
{
    client.queueSend(message);
}

const std::string   &CommandBase::getReplyTarget(Client &client) const
{
    static const std::string unknownTarget = "*";

    if (client.hasNickname())
        return (client.getNickname());
    return (unknownTarget);
}
