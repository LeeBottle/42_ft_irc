#ifndef MODE_HPP
# define MODE_HPP

# include "serverCommand/serverChannelCommand/CommandHelper.hpp"
# include "serverCommand/serverChannelCommand/mode/ModeApplier.hpp"
# include "serverCommand/serverChannelCommand/mode/ModeChecker.hpp"
# include "serverCommand/serverChannelCommand/mode/ModeParser.hpp"
# include "serverCommand/serverChannelCommand/mode/ModeChange.hpp"

// Stores one IRC channel, its members, topic, and mode state.
class Channel;
// Owns channels and coordinates channel lookup and removal.
class ChannelManager;
// Stores one connected IRC client and its protocol state.
class Client;
// Owns connected clients and provides client lookup operations.
class ClientManager;
// Splits one IRC line into command, parameters, and trailing text.
class Parser;

// Handles IRC channel and user mode commands.
class Mode
{
public:
    // Initializes this object with the supplied state.
    Mode(ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~Mode();

    // Validates and executes this IRC command.
    bool    handle(Client &, const Parser &);

private:
    ModeChecker  _checker;
    ModeParser   _parser;
    ModeApplier  _applier;

    // Initializes this object with the supplied state.
    Mode();
    // Initializes this object with the supplied state.
    Mode(const Mode &);
    // Performs the &operator= operation.
    Mode &operator=(const Mode &);

    // Performs the broadcast operation.
    void    broadcast(Client &, Channel &, const ModeChange &) const;
};

#endif
