#include "serverCommand/serverChannelCommand/mode/ModeParser.hpp"
#include "channel/Channel.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "parser/Parser.hpp"

#include <vector>


// Initializes this object with the supplied state.
ModeParser::ModeParser(ClientManager &clients, ChannelManager &channels)
    : _clients(clients)
{
    (void)channels;
}


// Destroys this object and releases its owned resources.
ModeParser::~ModeParser()
{
}


// Performs the collect operation.
bool    ModeParser::collect(Client &client, Channel &channel,
    const Parser &message, ModeChange &change)
{
    const std::vector<std::string>  &params = message.params();
    const std::string               &modeString = params[1];
    size_t                          index;

    index = 0;
    while (index < modeString.size())
    {
        if (modeString[index] == '+' || modeString[index] == '-')
            change.sign = modeString[index];
        else if (!collectLetter(client, channel, params, change,
                modeString[index]))
            return (false);
        ++index;
    }

    return (true);
}


// Performs the collect letter operation.
bool    ModeParser::collectLetter(Client &client, Channel &channel,
    const std::vector<std::string> &params, ModeChange &change, char mode)
{
    ModeOperation  operation;

    if (mode == 'i' || mode == 't')
    {
        operation.sign = change.sign;
        operation.mode = mode;
        change.operations.push_back(operation);
        change.addChange(mode);
    }
    else if (mode == 'k')
        return (collectKey(client, channel, params, change));
    else if (mode == 'l')
        return (collectLimit(client, channel, params, change));
    else if (mode == 'o')
        return (collectOperator(client, channel, params, change));
    else if (mode == 'b')
        sendBanEnd(client, channel);
    else
    {
        commandReply(client, ":ircserv 472 " + commandTarget(client)
            + " " + mode + " :is unknown mode char to me\r\n");

        return (false);
    }

    return (true);
}


// Performs the collect key operation.
bool    ModeParser::collectKey(Client &client, Channel &channel,
    const std::vector<std::string> &params, ModeChange &change)
{
    ModeOperation  operation;

    operation.sign = change.sign;
    operation.mode = 'k';
    if (change.sign == '-')
    {
        if (channel.modes().hasKey())
        {
            change.operations.push_back(operation);
            change.addChange('k');
        }
        return (true);
    }

    if (change.paramIndex >= params.size() 
        || params[change.paramIndex].empty())
    {
        commandReply(client, ":ircserv 461 " + commandTarget(client)
            + " MODE :Not enough parameters\r\n");

        return (false);
    }

    operation.value = params[change.paramIndex];
    change.operations.push_back(operation);
    change.addChange('k');
    change.params += " " + params[change.paramIndex++];

    return (true);
}


// Performs the collect limit operation.
bool    ModeParser::collectLimit(Client &client, Channel &channel,
    const std::vector<std::string> &params, ModeChange &change)
{
    ModeOperation  operation;

    operation.sign = change.sign;
    operation.mode = 'l';
    if (change.sign == '-')
    {
        if (channel.modes().hasLimit())
        {
            change.operations.push_back(operation);
            change.addChange('l');
        }
        return (true);
    }

    if (change.paramIndex >= params.size()
        || !parseLimit(params[change.paramIndex], operation.limit))
    {
        commandReply(client, ":ircserv 461 " + commandTarget(client)
            + " MODE :Not enough parameters\r\n");

        return (false);
    }

    change.operations.push_back(operation);
    change.addChange('l');
    change.params += " " + params[change.paramIndex++];

    return (true);
}


// Performs the collect operator operation.
bool    ModeParser::collectOperator(Client &client, Channel &channel,
    const std::vector<std::string> &params, ModeChange &change)
{
    ModeOperation  operation;

    if (change.paramIndex >= params.size())
    {
        commandReply(client, ":ircserv 461 " + commandTarget(client)
            + " MODE :Not enough parameters\r\n");

        return (false);
    }

    operation.sign = change.sign;
    operation.mode = 'o';
    operation.target = _clients.findByNickname(params[change.paramIndex]);

    if (operation.target == NULL || !channel.members().has(operation.target))
    {
        commandReply(client, ":ircserv 441 " + commandTarget(client)
            + " " + params[change.paramIndex] + " " + channel.name()
            + " :They aren't on that channel\r\n");

        return (false);
    }

    if ((change.sign == '+' && !channel.operators().has(operation.target))
        || (change.sign == '-' && channel.operators().has(operation.target)))
    {
        change.operations.push_back(operation);
        change.addChange('o');
        change.params += " " + operation.target->nickname();
    }

    ++change.paramIndex;

    return (true);
}


// Performs the send ban end operation.
void    ModeParser::sendBanEnd(Client &client, Channel &channel)
{
    commandReply(client, ":ircserv 368 " + commandTarget(client)
        + " " + channel.name() + " :End of Channel Ban List\r\n");
}


// Performs the parse limit operation.
bool    ModeParser::parseLimit(const std::string &value, size_t &limit) const
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
