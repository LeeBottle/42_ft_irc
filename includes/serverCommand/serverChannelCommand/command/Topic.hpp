#ifndef TOPIC_HPP
# define TOPIC_HPP

# include "serverCommand/serverChannelCommand/CommandHelper.hpp"

// Owns channels and coordinates channel lookup and removal.
class ChannelManager;
// Stores one connected IRC client and its protocol state.
class Client;
// Owns connected clients and provides client lookup operations.
class ClientManager;
// Splits one IRC line into command, parameters, and trailing text.
class Parser;

// Handles viewing, setting, and clearing channel topics.
class Topic
{
public:
    // Initializes this object with the supplied state.
    Topic(ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~Topic();

    // Validates and executes this IRC command.
    bool    handle(Client &, const Parser &);

private:
    ChannelManager  &_channels;

    // Initializes this object with the supplied state.
    Topic();
    // Initializes this object with the supplied state.
    Topic(const Topic &);
    // Performs the &operator= operation.
    Topic &operator=(const Topic &);
};

#endif
