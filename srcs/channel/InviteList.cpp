#include "channel/InviteList.hpp"

#include <cstddef>


// Initializes this object with the supplied state.
InviteList::InviteList() : _invites()
{
}


// Destroys this object and releases its owned resources.
InviteList::~InviteList()
{
}


// Reports whether s.
bool    InviteList::has(Client *client) const
{
    std::vector<Client *>::const_iterator   it;

    it = _invites.begin();
    while (it != _invites.end())
    {
        if (*it == client)
            return (true);
        ++it;
    }

    return (false);
}


// Adds an item to this collection.
void    InviteList::add(Client *client)
{
    if (client != NULL && !has(client))
        _invites.push_back(client);
}


// Removes a disconnected client from channels and client storage.
void    InviteList::remove(Client *client)
{
    std::vector<Client *>::iterator it;

    it = _invites.begin();
    while (it != _invites.end())
    {
        if (*it == client)
        {
            _invites.erase(it);
            return ;
        }
        ++it;
    }
}
