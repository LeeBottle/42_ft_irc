#include "channel/Channel.hpp"

Channel::Channel(const std::string &name)
    : _name(name), _members(), _operators(), _invites(), _modes()
{
}

Channel::~Channel()
{
}

const std::string   &Channel::getName() const
{
    return (_name);
}

const std::vector<Client *> &Channel::getClients() const
{
    return (_members.getClients());
}

std::string Channel::getMemberNames() const
{
    return (_members.getNames(_operators));
}

Channel::JoinResult  Channel::canJoin(Client *client,
    const std::string &key) const
{
    if (_modes.isInviteOnly() && !_invites.hasInvite(client)
        && !_members.hasClient(client))
        return (JOIN_INVITE_ONLY);

    if (_modes.hasKey() && key != _modes.getKey()
        && !_members.hasClient(client))
        return (JOIN_BAD_KEY);

    if (_modes.hasLimit() && _members.size() >= _modes.getLimit()
        && !_members.hasClient(client))
        return (JOIN_FULL);

    return (JOIN_ALLOWED);
}

void    Channel::addClient(Client *client)
{
    if (client == NULL || _members.hasClient(client))
        return ;

    _members.addClient(client);
    if (_members.size() == 1)
        _operators.addOperator(client);

    _invites.removeInvite(client);
}

void    Channel::removeClient(Client *client)
{
    _members.removeClient(client);
    _operators.removeOperator(client);
    _invites.removeInvite(client);
}

bool    Channel::hasClient(Client *client) const
{
    return (_members.hasClient(client));
}

void    Channel::inviteClient(Client *client)
{
    _invites.addInvite(client);
}

bool    Channel::isOperator(Client *client) const
{
    return (_operators.hasOperator(client));
}

bool    Channel::grantOperator(Client *client)
{
    if (!_members.hasClient(client))
        return (false);

    return (_operators.addOperator(client));
}

bool    Channel::revokeOperator(Client *client)
{
    return (_operators.removeOperator(client));
}

bool    Channel::canSetTopic(Client *client) const
{
    return (!_modes.isTopicRestricted() || _operators.hasOperator(client));
}

bool    Channel::isEmpty() const
{
    return (_members.isEmpty());
}

void    Channel::broadcast(Client &sender, const std::string &message)
{
    _members.broadcast(sender, message);
}

void    Channel::broadcastAll(const std::string &message)
{
    _members.broadcastAll(message);
}

ChannelModeState    &Channel::modes()
{
    return (_modes);
}

const ChannelModeState  &Channel::modes() const
{
    return (_modes);
}
