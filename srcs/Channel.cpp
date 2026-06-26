#include "Channel.hpp"

#include <algorithm>

Channel::Channel(const std::string &name)
    : _name(name), _members(), _topic()
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

bool    Channel::hasMember(Client *client) const
{
    return (std::find(_members.begin(), _members.end(), client)
        != _members.end());
}

bool    Channel::hasTopic() const
{
    return (!_topic.empty());
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
}

void    Channel::setTopic(const std::string& topic)
{
    _topic = topic;
}

bool    Channel::isEmpty() const
{
    return (_members.empty());
}
