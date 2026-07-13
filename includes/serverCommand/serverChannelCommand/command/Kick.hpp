#ifndef KICK_HPP
# define KICK_HPP

# include "serverCommand/serverChannelCommand/CommandHelper.hpp"

// Owns channels and coordinates channel lookup and removal.
class ChannelManager;
// Stores one connected IRC client and its protocol state.
class Client;
// Owns connected clients and provides client lookup operations.
class ClientManager;
// Splits one IRC line into command, parameters, and trailing text.
class Parser;

// Handles the IRC KICK command.
class Kick
{
public:
    // Initializes this object with the supplied state.
    Kick(ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~Kick();

    // Validates and executes this IRC command.
    bool    handle(Client &, const Parser &);

private:
    ClientManager   &_clients;
    ChannelManager  &_channels;

    // Initializes this object with the supplied state.
    Kick();
    // Initializes this object with the supplied state.
    Kick(const Kick &);
    // Performs the &operator= operation.
    Kick &operator=(const Kick &);
};

#endif
