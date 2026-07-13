#include "serverCommand/serverChannelCommand/mode/Mode.hpp"
#include "channel/Channel.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "parser/Parser.hpp"


// Initializes this object with the supplied state.
Mode::Mode(ClientManager &clients, ChannelManager &channels)
    : _checker(clients, channels),
      _parser(clients, channels),
      _applier()
{
}


// Destroys this object and releases its owned resources.
Mode::~Mode()
{
}


// Validates and executes this IRC command.
bool    Mode::handle(Client &client, const Parser &message)
{
    Channel                 *channel;
    ModeChange              change;

    channel = NULL;
    if (_checker.prepare(client, message, channel))
        return (true);

    if (!_parser.collect(client, *channel, message, change))
        return (true);

    _applier.apply(*channel, change);
    broadcast(client, *channel, change);

    return (true);
}


// Performs the broadcast operation.
void    Mode::broadcast(Client &client, Channel &channel,
    const ModeChange &change) const
{
    if (!change.changes.empty())
        commandToAll(channel, ":" + client.prefix() + " MODE "
            + channel.name() + " " + change.changes + change.params + "\r\n");
}
