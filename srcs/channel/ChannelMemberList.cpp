#include "channel/ChannelMemberList.hpp"
#include "channel/ChannelOperatorList.hpp"
#include "client/Client.hpp"

#include <cstddef>

ChannelMemberList::ChannelMemberList()
    : _clients()
{
}

ChannelMemberList::~ChannelMemberList()
{
}

const std::vector<Client *> &ChannelMemberList::getClients() const
{
    return (_clients);
}

size_t  ChannelMemberList::size() const
{
    return (_clients.size());
}

bool    ChannelMemberList::isEmpty() const
{
    return (_clients.empty());
}

bool    ChannelMemberList::hasClient(Client *client) const
{
    std::vector<Client *>::const_iterator   it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if (*it == client)
            return (true);
        ++it;
    }
    return (false);
}

std::string ChannelMemberList::getNames(const ChannelOperatorList &operators) const
{
    std::vector<Client *>::const_iterator   it;
    std::string                             names;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if ((*it)->hasNickname())
        {
            if (!names.empty())
                names += " ";
            if (operators.hasOperator(*it))
                names += "@";
            names += (*it)->getNickname();
        }
        ++it;
    }
    return (names);
}

void    ChannelMemberList::addClient(Client *client)
{
    if (client == NULL || hasClient(client))
        return ;
    _clients.push_back(client);
}

void    ChannelMemberList::removeClient(Client *client)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if (*it == client)
        {
            _clients.erase(it);
            return ;
        }
        ++it;
    }
}

void    ChannelMemberList::broadcast(Client &sender, const std::string &message)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if (*it != &sender)
            (*it)->queueSend(message);
        ++it;
    }
}

void    ChannelMemberList::broadcastAll(const std::string &message)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        (*it)->queueSend(message);
        ++it;
    }
}
