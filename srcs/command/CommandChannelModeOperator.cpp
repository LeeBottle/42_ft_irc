#include "command/CommandChannelModeOperator.hpp"
#include "channel/Channel.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "command/CommandChannelModeEdit.hpp"
#include "command/CommandChannelModeParameter.hpp"

#include <cstddef>

CommandChannelModeOperator::CommandChannelModeOperator(
    ChannelManager &channels, ClientManager &clients)
    : CommandChannelBase(channels, clients)
{
}

CommandChannelModeOperator::~CommandChannelModeOperator()
{
}

bool    CommandChannelModeOperator::applyOperatorMode(Client &client,
    Channel &channel, const std::vector<std::string> &params,
    CommandChannelModeEdit &edit)
{
    Client  *targetClient;

    if (edit.paramIndex >= params.size())
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " MODE :Not enough parameters\r\n");
        return (false);
    }
    targetClient = _clients.findByNickname(params[edit.paramIndex]);
    if (targetClient == NULL || !channel.hasClient(targetClient))
    {
        queueReply(client, ":ircserv 441 " + getReplyTarget(client)
            + " " + params[edit.paramIndex] + " " + channel.getName()
            + " :They aren't on that channel\r\n");
        return (false);
    }
    if ((edit.sign == '+' && channel.grantOperator(targetClient))
        || (edit.sign == '-' && channel.revokeOperator(targetClient)))
    {
        edit.addChange('o');
        edit.params += " " + targetClient->getNickname();
    }
    ++edit.paramIndex;
    return (true);
}
