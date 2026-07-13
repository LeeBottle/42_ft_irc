#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>

# include "channel/InviteList.hpp"
# include "channel/ModeState.hpp"
# include "channel/MemberList.hpp"
# include "channel/OperatorList.hpp"

// Stores one connected IRC client and its protocol state.
class Client;

// Stores one IRC channel, its members, topic, and mode state.
class Channel
{
public:
    // Initializes this object with the supplied state.
    Channel(const std::string &);
    // Destroys this object and releases its owned resources.
    ~Channel();

    // Performs the &name operation.
    const std::string   &name() const;
    
    // Performs the &members operation.
    MemberList    &members();
    // Performs the &operators operation.
    OperatorList  &operators();
    // Performs the &invites operation.
    InviteList    &invites();
    // Performs the &modes operation.
    ModeState     &modes();

private:
    std::string     _name;
    MemberList      _members;
    OperatorList    _operators;
    InviteList      _invites;
    ModeState       _modes;

    // Initializes this object with the supplied state.
    Channel();
    // Initializes this object with the supplied state.
    Channel(const Channel &);
    // Performs the &operator= operation.
    Channel &operator=(const Channel &);
};

#endif
