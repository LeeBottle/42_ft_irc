#include "command/CommandChannelModeApply.hpp"
#include "channel/Channel.hpp"
#include "client/Client.hpp"
#include "command/Command.hpp"
#include "command/CommandChannelModeEdit.hpp"
#include "command/CommandChannelModeOperator.hpp"
#include "command/CommandChannelModeParameter.hpp"

CommandChannelModeApply::CommandChannelModeApply(ChannelManager &channels,
    ClientManager &clients, CommandChannelModeParameter &parameter,
    CommandChannelModeOperator &operatorMode)
    : CommandChannelBase(channels, clients), _parameter(parameter),
      _operator(operatorMode)
{
}

CommandChannelModeApply::~CommandChannelModeApply()
{
}

bool    CommandChannelModeApply::applyModeString(Client &client,
    Channel &channel, const Command &command, CommandChannelModeEdit &edit)
{
    const std::vector<std::string>  &params = command.getParams();
    const std::string               &modeString = params[1];
    size_t                          index;

    index = 0;
    while (index < modeString.size())
    {
        if (modeString[index] == '+' || modeString[index] == '-')
            edit.sign = modeString[index];
        else if (!applyModeLetter(client, channel, params, edit,
                modeString[index]))
            return (false);
        ++index;
    }
    return (true);
}

bool    CommandChannelModeApply::applyModeLetter(Client &client,
    Channel &channel, const std::vector<std::string> &params,
    CommandChannelModeEdit &edit, char mode)
{
    if (mode == 'i' || mode == 't')
        applySimpleMode(channel, edit, mode);
    else if (mode == 'k')
        return (_parameter.applyKeyMode(client, channel, params, edit));
    else if (mode == 'l')
        return (_parameter.applyLimitMode(client, channel, params, edit));
    else if (mode == 'o')
        return (_operator.applyOperatorMode(client, channel, params, edit));
    else if (mode == 'b')
        sendBanListEnd(client, channel);
    else
    {
        queueReply(client, ":ircserv 472 " + getReplyTarget(client)
            + " " + mode + " :is unknown mode char to me\r\n");
        return (false);
    }
    return (true);
}

void    CommandChannelModeApply::applySimpleMode(Channel &channel,
    CommandChannelModeEdit &edit, char mode) const
{
    bool    changed;

    changed = false;
    if (mode == 'i' && edit.sign == '+')
        changed = channel.modes().setInviteOnly(true);
    else if (mode == 'i' && edit.sign == '-')
        changed = channel.modes().setInviteOnly(false);
    else if (mode == 't' && edit.sign == '+')
        changed = channel.modes().setTopicRestricted(true);
    else if (mode == 't' && edit.sign == '-')
        changed = channel.modes().setTopicRestricted(false);
    if (changed)
        edit.addChange(mode);
}

void    CommandChannelModeApply::sendBanListEnd(Client &client,
    Channel &channel)
{
    queueReply(client, ":ircserv 368 " + getReplyTarget(client)
        + " " + channel.getName() + " :End of Channel Ban List\r\n");
}

void    CommandChannelModeApply::broadcastModeChanges(Client &client,
    Channel &channel, const CommandChannelModeEdit &edit) const
{
    if (!edit.changes.empty())
        channel.broadcastAll(":" + client.getPrefix() + " MODE "
            + channel.getName() + " " + edit.changes + edit.params + "\r\n");
}
