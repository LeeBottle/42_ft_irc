#include "command/CommandChannelMode.hpp"
#include "channel/Channel.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "command/Command.hpp"

#include <vector>

CommandChannelMode::CommandChannelMode(ChannelManager &channels,
    ClientManager &clients)
    : CommandChannelBase(channels, clients)
{
}

CommandChannelMode::~CommandChannelMode()
{
}

bool    CommandChannelMode::parseLimit(const std::string &value,
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

void    CommandChannelMode::addModeChange(std::string &changes,
    char &currentSign, char sign, char mode) const
{
    if (currentSign != sign)
    {
        changes += sign;
        currentSign = sign;
    }
    changes += mode;
}

void    CommandChannelMode::handleMode(Client &client, const Command &command)
{
    const std::vector<std::string>  &params = command.getParams();
    Channel                         *channel;
    Client                          *targetClient;
    std::string                     modeChanges;
    std::string                     modeParams;
    std::string                     modeString;
    size_t                          index;
    size_t                          paramIndex;
    size_t                          limit;
    char                            sign;
    char                            currentSign;

    if (!client.isRegistered())
    {
        queueReply(client, ":ircserv 451 " + getReplyTarget(client)
            + " :You have not registered\r\n");
        return ;
    }
    if (params.empty())
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " MODE :Not enough parameters\r\n");
        return ;
    }
    channel = _channels.find(params[0]);
    if (channel == NULL)
    {
        queueReply(client, ":ircserv 403 " + getReplyTarget(client)
            + " " + params[0] + " :No such channel\r\n");
        return ;
    }
    if (params.size() == 1)
    {
        queueReply(client, ":ircserv 324 " + getReplyTarget(client)
            + " " + channel->getName() + " " + channel->modes().getModeString()
            + channel->modes().getModeParameters() + "\r\n");
        return ;
    }
    if (!channel->hasClient(&client))
    {
        queueReply(client, ":ircserv 442 " + getReplyTarget(client)
            + " " + params[0] + " :You're not on that channel\r\n");
        return ;
    }
    if (!channel->isOperator(&client))
    {
        queueReply(client, ":ircserv 482 " + getReplyTarget(client)
            + " " + params[0] + " :You're not channel operator\r\n");
        return ;
    }
    modeString = params[1];
    index = 0;
    paramIndex = 2;
    sign = '+';
    currentSign = '\0';
    while (index < modeString.size())
    {
        if (modeString[index] == '+' || modeString[index] == '-')
            sign = modeString[index];
        else if (modeString[index] == 'i')
        {
            if ((sign == '+' && channel->modes().setInviteOnly(true))
                || (sign == '-' && channel->modes().setInviteOnly(false)))
                addModeChange(modeChanges, currentSign, sign, 'i');
        }
        else if (modeString[index] == 't')
        {
            if ((sign == '+' && channel->modes().setTopicRestricted(true))
                || (sign == '-' && channel->modes().setTopicRestricted(false)))
                addModeChange(modeChanges, currentSign, sign, 't');
        }
        else if (modeString[index] == 'k')
        {
            if (sign == '+')
            {
                if (paramIndex >= params.size() || params[paramIndex].empty())
                {
                    queueReply(client, ":ircserv 461 " + getReplyTarget(client)
                        + " MODE :Not enough parameters\r\n");
                    return ;
                }
                channel->modes().setKey(params[paramIndex]);
                addModeChange(modeChanges, currentSign, sign, 'k');
                modeParams += " " + params[paramIndex++];
            }
            else if (channel->modes().clearKey())
                addModeChange(modeChanges, currentSign, sign, 'k');
        }
        else if (modeString[index] == 'l')
        {
            if (sign == '+')
            {
                if (paramIndex >= params.size()
                    || !parseLimit(params[paramIndex], limit))
                {
                    queueReply(client, ":ircserv 461 " + getReplyTarget(client)
                        + " MODE :Not enough parameters\r\n");
                    return ;
                }
                channel->modes().setLimit(limit);
                addModeChange(modeChanges, currentSign, sign, 'l');
                modeParams += " " + params[paramIndex++];
            }
            else if (channel->modes().clearLimit())
                addModeChange(modeChanges, currentSign, sign, 'l');
        }
        else if (modeString[index] == 'o')
        {
            if (paramIndex >= params.size())
            {
                queueReply(client, ":ircserv 461 " + getReplyTarget(client)
                    + " MODE :Not enough parameters\r\n");
                return ;
            }
            targetClient = _clients.findByNickname(params[paramIndex]);
            if (targetClient == NULL || !channel->hasClient(targetClient))
            {
                queueReply(client, ":ircserv 441 " + getReplyTarget(client)
                    + " " + params[paramIndex] + " " + params[0]
                    + " :They aren't on that channel\r\n");
                return ;
            }
            if ((sign == '+' && channel->grantOperator(targetClient))
                || (sign == '-' && channel->revokeOperator(targetClient)))
            {
                addModeChange(modeChanges, currentSign, sign, 'o');
                modeParams += " " + targetClient->getNickname();
            }
            ++paramIndex;
        }
        else if (modeString[index] == 'b')
        {
            queueReply(client, ":ircserv 368 " + getReplyTarget(client)
                + " " + channel->getName()
                + " :End of Channel Ban List\r\n");
        }
        else
        {
            queueReply(client, ":ircserv 472 " + getReplyTarget(client)
                + " " + modeString[index]
                + " :is unknown mode char to me\r\n");
            return ;
        }
        ++index;
    }
    if (!modeChanges.empty())
        channel->broadcastAll(":" + client.getPrefix() + " MODE "
            + channel->getName() + " " + modeChanges + modeParams + "\r\n");
}
