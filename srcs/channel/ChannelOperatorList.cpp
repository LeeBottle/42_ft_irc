#include "channel/ChannelOperatorList.hpp"

#include <cstddef>

ChannelOperatorList::ChannelOperatorList()
    : _operators()
{
}

ChannelOperatorList::~ChannelOperatorList()
{
}

bool    ChannelOperatorList::hasOperator(Client *client) const
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

bool    ChannelOperatorList::addOperator(Client *client)
{
    if (client == NULL || hasOperator(client))
        return (false);
    _operators.push_back(client);
    return (true);
}

bool    ChannelOperatorList::removeOperator(Client *client)
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
