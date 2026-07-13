#ifndef JOIN_HPP
# define JOIN_HPP

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

// Handles the IRC JOIN command.
class Join
{
public:
    // Initializes this object with the supplied state.
    Join(ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~Join();

    // Validates and executes this IRC command.
    bool    handle(Client &, const Parser &);

private:
    enum JoinResult
    {
        JOIN_ALLOWED,
        JOIN_INVITE_ONLY,
        JOIN_BAD_KEY,
        JOIN_FULL
    };

    ChannelManager  &_channels;

    // Initializes this object with the supplied state.
    Join();
    // Initializes this object with the supplied state.
    Join(const Join &);
    // Performs the &operator= operation.
    Join &operator=(const Join &);

    // Checks permission.
    JoinResult  checkPermission(Channel &, Client &, const std::string &) const;
    // Performs the join operation.
    void        join(Channel &, Client &) const;
};

#endif
