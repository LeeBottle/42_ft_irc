#ifndef PART_HPP
# define PART_HPP

# include "serverCommand/serverChannelCommand/CommandHelper.hpp"

// Owns channels and coordinates channel lookup and removal.
class ChannelManager;
// Stores one connected IRC client and its protocol state.
class Client;
// Owns connected clients and provides client lookup operations.
class ClientManager;
// Splits one IRC line into command, parameters, and trailing text.
class Parser;

// Handles the IRC PART command.
class Part
{
public:
    // Initializes this object with the supplied state.
    Part(ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~Part();

    // Validates and executes this IRC command.
    bool    handle(Client &, const Parser &);

private:
    ChannelManager  &_channels;

    // Initializes this object with the supplied state.
    Part();
    // Initializes this object with the supplied state.
    Part(const Part &);
    // Performs the &operator= operation.
    Part &operator=(const Part &);
};

#endif
