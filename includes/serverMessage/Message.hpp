#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>

# include "serverCommand/Command.hpp"

// Owns channels and coordinates channel lookup and removal.
class ChannelManager;
// Stores one connected IRC client and its protocol state.
class Client;
// Owns connected clients and provides client lookup operations.
class ClientManager;
// Splits one IRC line into command, parameters, and trailing text.
class Parser;

// Extracts complete lines, invokes the parser, and dispatches commands.
class Message
{
public:
    // Initializes this object with the supplied state.
    Message(const std::string &, ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~Message();

    // Processes complete IRC messages currently buffered for a client.
    bool    process(Client &);

private:
    Command   _command;

    // Initializes this object with the supplied state.
    Message();
    // Initializes this object with the supplied state.
    Message(const Message &);
    // Performs the &operator= operation.
    Message &operator=(const Message &);

    // Validates and executes this IRC command.
    bool    handle(Client &, const std::string &);
};

#endif
