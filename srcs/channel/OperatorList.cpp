#include "channel/OperatorList.hpp"

#include <cstddef>


// Initializes this object with the supplied state.
OperatorList::OperatorList() : _operators()
{
}


// Destroys this object and releases its owned resources.
OperatorList::~OperatorList()
{
}


// Reports whether s.
bool    OperatorList::has(Client *client) const
{
    std::vector<Client *>::const_iterator   it;

    it = _operators.begin();
    while (it != _operators.end())
    {
        if (*it == client)
            return (true);
        ++it;
    }

    return (false);
}


// Adds an item to this collection.
bool    OperatorList::add(Client *client)
{
    if (client == NULL || has(client))
        return (false);

    _operators.push_back(client);

    return (true);
}


// Removes a disconnected client from channels and client storage.
bool    OperatorList::remove(Client *client)
{
    std::vector<Client *>::iterator it;

    it = _operators.begin();
    while (it != _operators.end())
    {
        if (*it == client)
        {
            _operators.erase(it);
            return (true);
        }
        ++it;
    }

    return (false);
}
