#ifndef WHO_HPP
# define WHO_HPP

# include "serverCommand/serverChannelCommand/CommandHelper.hpp"

// Owns channels and coordinates channel lookup and removal.
class ChannelManager;
// Stores one connected IRC client and its protocol state.
class Client;
// Owns connected clients and provides client lookup operations.
class ClientManager;
// Splits one IRC line into command, parameters, and trailing text.
class Parser;

// Handles the IRC WHO command and replies.
class Who
{
public:
    // Initializes this object with the supplied state.
    Who(ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~Who();

    // Validates and executes this IRC command.
    bool    handle(Client &, const Parser &);

private:
    ChannelManager  &_channels;

    // Initializes this object with the supplied state.
    Who();
    // Initializes this object with the supplied state.
    Who(const Who &);
    // Performs the &operator= operation.
    Who &operator=(const Who &);
};

#endif
