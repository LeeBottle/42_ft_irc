#include "Channel.hpp"

#include <algorithm>

Channel::Channel(const std::string &name)
    : _name(name), _members()
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

bool    Channel::hasMember(Client *client) const
{
    return (std::find(_members.begin(), _members.end(), client)
        != _members.end());
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

bool    Channel::isEmpty() const
{
    return (_members.empty());
}
