#include "command/CommandChannelModeParameter.hpp"
#include "channel/Channel.hpp"
#include "client/Client.hpp"
#include "command/CommandChannelModeEdit.hpp"

CommandChannelModeParameter::CommandChannelModeParameter(
    ChannelManager &channels, ClientManager &clients)
    : CommandChannelBase(channels, clients)
{
}

CommandChannelModeParameter::~CommandChannelModeParameter()
{
}

bool    CommandChannelModeParameter::parseLimit(const std::string &value,
    size_t &limit) const
{
    size_t index;

    if (value.empty())
        return (false);
    limit = 0;
    index = 0;
    while (index < value.size())
    {
        if (value[index] < '0' || value[index] > '9')
            return (false);
        limit = limit * 10 + static_cast<size_t>(value[index] - '0');
        ++index;
    }
    return (limit > 0);
}

bool    CommandChannelModeParameter::applyKeyMode(Client &client,
    Channel &channel, const std::vector<std::string> &params,
    CommandChannelModeEdit &edit)
{
    if (edit.sign == '-')
    {
        if (channel.modes().clearKey())
            edit.addChange('k');
        return (true);
    }
    if (edit.paramIndex >= params.size() || params[edit.paramIndex].empty())
    {
        sendParameterError(client);
        return (false);
    }
    channel.modes().setKey(params[edit.paramIndex]);
    edit.addChange('k');
    edit.params += " " + params[edit.paramIndex++];
    return (true);
}

bool    CommandChannelModeParameter::applyLimitMode(Client &client,
    Channel &channel, const std::vector<std::string> &params,
    CommandChannelModeEdit &edit)
{
    size_t  limit;

    if (edit.sign == '-')
    {
        if (channel.modes().clearLimit())
            edit.addChange('l');
        return (true);
    }
    if (edit.paramIndex >= params.size()
        || !parseLimit(params[edit.paramIndex], limit))
    {
        sendParameterError(client);
        return (false);
    }
    channel.modes().setLimit(limit);
    edit.addChange('l');
    edit.params += " " + params[edit.paramIndex++];
    return (true);
}

void    CommandChannelModeParameter::sendParameterError(Client &client)
{
    queueReply(client, ":ircserv 461 " + getReplyTarget(client)
        + " MODE :Not enough parameters\r\n");
}
