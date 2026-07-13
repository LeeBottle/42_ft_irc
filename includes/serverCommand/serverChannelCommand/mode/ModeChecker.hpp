#ifndef MODECHECKER_HPP
# define MODECHECKER_HPP

# include "serverCommand/serverChannelCommand/CommandHelper.hpp"

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

// Validates permissions and arguments for mode changes.
class ModeChecker
{
public:
    // Initializes this object with the supplied state.
    ModeChecker(ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~ModeChecker();

    // Performs the prepare operation.
    bool    prepare(Client &, const Parser &, Channel *&);

private:
    ChannelManager  &_channels;

    // Initializes this object with the supplied state.
    ModeChecker();
    // Initializes this object with the supplied state.
    ModeChecker(const ModeChecker &);
    // Performs the &operator= operation.
    ModeChecker &operator=(const ModeChecker &);

    // Performs the user operation.
    bool    user(Client &, const Parser &);
    // Performs the ban list operation.
    bool    banList(Client &, const Parser &, Channel *&);
};

#endif
