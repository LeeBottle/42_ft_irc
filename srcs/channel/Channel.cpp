#include "channel/Channel.hpp"


// Initializes this object with the supplied state.
Channel::Channel(const std::string &name)
    : _name(name),
      _members(),
      _operators(),
      _invites(),
      _modes()
{
}


// Destroys this object and releases its owned resources.
Channel::~Channel()
{
}


// Performs the name operation.
const std::string   &Channel::name() const
{
    return (_name);
}


// Performs the members operation.
MemberList   &Channel::members()
{
    return (_members);
}


// Performs the operators operation.
OperatorList &Channel::operators()
{
    return (_operators);
}


// Performs the invites operation.
InviteList   &Channel::invites()
{
    return (_invites);
}


// Performs the modes operation.
ModeState    &Channel::modes()
{
    return (_modes);
}
