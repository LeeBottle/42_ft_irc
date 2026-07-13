#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>

# include "serverCommand/serverChannelCommand/command/Invite.hpp"
# include "serverCommand/serverChannelCommand/command/Join.hpp"
# include "serverCommand/serverChannelCommand/command/Kick.hpp"
# include "serverCommand/serverChannelCommand/command/Names.hpp"
# include "serverCommand/serverChannelCommand/command/Part.hpp"
# include "serverCommand/serverChannelCommand/command/Privmsg.hpp"
# include "serverCommand/serverChannelCommand/command/Topic.hpp"
# include "serverCommand/serverChannelCommand/command/Who.hpp"
# include "serverCommand/serverChannelCommand/mode/Mode.hpp"
# include "serverCommand/serverClientCommand/ClientCommand.hpp"

// Owns channels and coordinates channel lookup and removal.
class ChannelManager;
// Stores one connected IRC client and its protocol state.
class Client;
// Owns connected clients and provides client lookup operations.
class ClientManager;
// Splits one IRC line into command, parameters, and trailing text.
class Parser;

// Routes parsed IRC messages to command implementations.
class Command
{
public:
    // Initializes this object with the supplied state.
    Command(const std::string &, ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~Command();

    // Executes this IRC command for the given client.
    bool    execute(Client &, const Parser &);

private:
    ClientCommand   _client;
    Join            _join;
    Part            _part;
    Privmsg         _privmsg;
    Names           _names;
    Who             _who;
    Topic           _topic;
    Invite          _invite;
    Kick            _kick;
    Mode            _mode;

    // Initializes this object with the supplied state.
    Command();
    // Initializes this object with the supplied state.
    Command(const Command &);
    // Performs the &operator= operation.
    Command &operator=(const Command &);
};

#endif
