#include "command/CommandModeApply.hpp"
#include "channel/Channel.hpp"
#include "client/Client.hpp"
#include "parser/Parser.hpp"
#include "command/CommandModeEdit.hpp"
#include "command/CommandModeOperator.hpp"
#include "command/CommandModeParameter.hpp"

CommandModeApply::CommandModeApply(ChannelManager &channels,
    ClientManager &clients, CommandModeParameter &parameter,
    CommandModeOperator &operatorMode)
    : CommandBase(channels, clients), _parameter(parameter),
      _operator(operatorMode)
{
}

CommandModeApply::~CommandModeApply()
{
}

bool    CommandModeApply::applyModeString(Client &client,
    Channel &channel, const Parser &message, CommandModeEdit &edit)
{
    const std::vector<std::string>  &params = message.getParams();
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

bool    CommandModeApply::applyModeLetter(Client &client,
    Channel &channel, const std::vector<std::string> &params,
    CommandModeEdit &edit, char mode)
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

void    CommandModeApply::applySimpleMode(Channel &channel,
    CommandModeEdit &edit, char mode) const
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

void    CommandModeApply::sendBanListEnd(Client &client,
    Channel &channel)
{
    queueReply(client, ":ircserv 368 " + getReplyTarget(client)
        + " " + channel.getName() + " :End of Channel Ban List\r\n");
}

void    CommandModeApply::broadcastModeChanges(Client &client,
    Channel &channel, const CommandModeEdit &edit) const
{
    if (!edit.changes.empty())
        channel.broadcastAll(":" + client.getPrefix() + " MODE "
            + channel.getName() + " " + edit.changes + edit.params + "\r\n");
}
