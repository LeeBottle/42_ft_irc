#include "channel/ChannelInviteList.hpp"

#include <cstddef>

ChannelInviteList::ChannelInviteList()
    : _invitedClients()
{
}

ChannelInviteList::~ChannelInviteList()
{
}

bool    ChannelInviteList::hasInvite(Client *client) const
{
    std::vector<Client *>::const_iterator   it;

    it = _invitedClients.begin();
    while (it != _invitedClients.end())
    {
        if (*it == client)
            return (true);
        ++it;
    }
    return (false);
}

void    ChannelInviteList::addInvite(Client *client)
{
    if (client != NULL && !hasInvite(client))
        _invitedClients.push_back(client);
}

void    ChannelInviteList::removeInvite(Client *client)
{
    std::vector<Client *>::iterator it;

    it = _invitedClients.begin();
    while (it != _invitedClients.end())
    {
        if (*it == client)
        {
            _invitedClients.erase(it);
            return ;
        }
        ++it;
    }
}
