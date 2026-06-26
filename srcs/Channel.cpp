#include "Channel.hpp"

#include <algorithm>

Channel::Channel(const std::string &name)
    : _name(name), _members(), _operators(), _invitedClients(), _topic(),
    _inviteOnly(false), _topicRestricted(false), _hasKey(false), _key(),
    _hasLimit(false), _limit(0)
{
}

Channel::~Channel()
{
}

const std::string   &Channel::getName() const
{
    return (_name);
}

const std::vector<Client *> &Channel::getMembers() const
{
    return (_members);
}

const std::string   &Channel::getTopic() const
{
    return (_topic);
}

const std::string   &Channel::getKey() const
{
    return (_key);
}

unsigned int    Channel::getLimit() const
{
    return (_limit);
}

bool    Channel::hasMember(Client *client) const
{
    return (std::find(_members.begin(), _members.end(), client)
        != _members.end());
}

bool    Channel::hasOperator(Client *client) const
{
    return (std::find(_operators.begin(), _operators.end(), client)
        != _operators.end());
}

bool    Channel::hasInvitation(Client *client) const
{
    return (std::find(_invitedClients.begin(), _invitedClients.end(), client)
        != _invitedClients.end());
}

bool    Channel::hasTopic() const
{
    return (!_topic.empty());
}

bool    Channel::isInviteOnly() const
{
    return (_inviteOnly);
}

bool    Channel::isTopicRestricted() const
{
    return (_topicRestricted);
}

bool    Channel::hasKey() const
{
    return (_hasKey);
}

bool    Channel::hasLimit() const
{
    return (_hasLimit);
}

void    Channel::addMember(Client *client)
{
    if (client == NULL)
        return ;
    if (hasMember(client))
        return ;
    _members.push_back(client);
}

void    Channel::removeMember(Client *client)
{
    std::vector<Client *>::iterator it;

    it = std::find(_members.begin(), _members.end(), client);
    if (it != _members.end())
        _members.erase(it);
    removeOperator(client);
    removeInvitation(client);
}

void    Channel::addOperator(Client *client)
{
    if (client == NULL || hasOperator(client))
        return ;
    _operators.push_back(client);
}

void    Channel::removeOperator(Client *client)
{
    std::vector<Client *>::iterator it;

    it = std::find(_operators.begin(), _operators.end(), client);
    if (it != _operators.end())
        _operators.erase(it);
}

void    Channel::addInvitation(Client *client)
{
    if (client == NULL || hasInvitation(client))
        return ;
    _invitedClients.push_back(client);
}

void    Channel::removeInvitation(Client *client)
{
    std::vector<Client *>::iterator it;

    it = std::find(_invitedClients.begin(), _invitedClients.end(), client);
    if (it != _invitedClients.end())
        _invitedClients.erase(it);
}

void    Channel::setTopic(const std::string &topic)
{
    _topic = topic;
}

void    Channel::setInviteOnly(bool value)
{
    _inviteOnly = value;
}

void    Channel::setTopicRestricted(bool value)
{
    _topicRestricted = value;
}

void    Channel::setKey(const std::string &key)
{
    _hasKey = true;
    _key = key;
}

void    Channel::clearKey()
{
    _hasKey = false;
    _key.clear();
}

void    Channel::setLimit(unsigned int limit)
{
    _hasLimit = true;
    _limit = limit;
}

void    Channel::clearLimit()
{
    _hasLimit = false;
    _limit = 0;
}

bool    Channel::isEmpty() const
{
    return (_members.empty());
}
