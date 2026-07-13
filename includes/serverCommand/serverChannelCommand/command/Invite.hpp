#ifndef INVITE_HPP
# define INVITE_HPP

# include "serverCommand/serverChannelCommand/CommandHelper.hpp"

// Owns channels and coordinates channel lookup and removal.
class ChannelManager;
// Stores one connected IRC client and its protocol state.
class Client;
// Owns connected clients and provides client lookup operations.
class ClientManager;
// Splits one IRC line into command, parameters, and trailing text.
class Parser;

// Handles the IRC INVITE command.
class Invite
{
public:
    // Initializes this object with the supplied state.
    Invite(ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~Invite();

    // Validates and executes this IRC command.
    bool    handle(Client &, const Parser &);

private:
    ClientManager   &_clients;
    ChannelManager  &_channels;

    // Initializes this object with the supplied state.
    Invite();
    // Initializes this object with the supplied state.
    Invite(const Invite &);
    // Performs the &operator= operation.
    Invite &operator=(const Invite &);
};

#endif
