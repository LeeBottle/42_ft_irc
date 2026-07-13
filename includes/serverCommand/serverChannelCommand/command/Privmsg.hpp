#ifndef PRIVMSG_HPP
# define PRIVMSG_HPP

# include "serverCommand/serverChannelCommand/CommandHelper.hpp"

// Owns channels and coordinates channel lookup and removal.
class ChannelManager;
// Stores one connected IRC client and its protocol state.
class Client;
// Owns connected clients and provides client lookup operations.
class ClientManager;
// Splits one IRC line into command, parameters, and trailing text.
class Parser;

// Handles private and channel IRC messages.
class Privmsg
{
public:
    // Initializes this object with the supplied state.
    Privmsg(ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~Privmsg();

    // Validates and executes this IRC command.
    bool    handle(Client &, const Parser &);

private:
    ChannelManager  &_channels;

    // Initializes this object with the supplied state.
    Privmsg();
    // Initializes this object with the supplied state.
    Privmsg(const Privmsg &);
    // Performs the &operator= operation.
    Privmsg &operator=(const Privmsg &);

    // Performs the send to channel operation.
    void    sendToChannel(Client &, const std::string &, const std::string &);
};

#endif
