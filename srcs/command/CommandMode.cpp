#include "command/CommandMode.hpp"
#include "channel/Channel.hpp"
#include "command/CommandModeApply.hpp"
#include "command/CommandModeEdit.hpp"
#include "command/CommandModeOperator.hpp"
#include "command/CommandModeParameter.hpp"
#include "command/CommandModePrepare.hpp"

CommandMode::CommandMode(ChannelManager &channels,
    ClientManager &clients)
    : CommandBase(channels, clients), 
      _prepare(channels, clients), 
      _parameter(channels, clients),
      _operator(channels, clients), 
      _apply(channels, clients, _parameter, _operator)
{
}

CommandMode::~CommandMode()
{
}

void    CommandMode::executeMode(Client &client, const Parser &message)
{
    Channel                 *channel;
    CommandModeEdit  edit;

    channel = NULL;
    if (_prepare.shouldStop(client, message, channel))
        return ;
    if (!_apply.applyModeString(client, *channel, message, edit))
        return ;
    _apply.broadcastModeChanges(client, *channel, edit);
}
