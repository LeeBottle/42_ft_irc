#include "command/CommandChannelMode.hpp"
#include "channel/Channel.hpp"
#include "command/CommandChannelModeApply.hpp"
#include "command/CommandChannelModeEdit.hpp"
#include "command/CommandChannelModeOperator.hpp"
#include "command/CommandChannelModeParameter.hpp"
#include "command/CommandChannelModePrepare.hpp"

CommandChannelMode::CommandChannelMode(ChannelManager &channels,
    ClientManager &clients)
    : CommandChannelBase(channels, clients), 
      _prepare(channels, clients), 
      _parameter(channels, clients),
      _operator(channels, clients), 
      _apply(channels, clients, _parameter, _operator)
{
}

CommandChannelMode::~CommandChannelMode()
{
}

void    CommandChannelMode::handleMode(Client &client, const Command &command)
{
    Channel                 *channel;
    CommandChannelModeEdit  edit;

    channel = NULL;
    if (_prepare.shouldStop(client, command, channel))
        return ;
    if (!_apply.applyModeString(client, *channel, command, edit))
        return ;
    _apply.broadcastModeChanges(client, *channel, edit);
}
