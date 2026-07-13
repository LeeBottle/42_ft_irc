#include "channel/ChannelManager.hpp"
#include "channel/Channel.hpp"


// Initializes this object with the supplied state.
ChannelManager::ChannelManager() : _channels()
{
}


// Destroys this object and releases its owned resources.
ChannelManager::~ChannelManager()
{
    clear();
}


// Finds an existing channel or creates it when missing.
Channel *ChannelManager::findOrCreate(const std::string &name)
{
    Channel *found;
    Channel *created;

    found = find(name);
    if (found != NULL)
        return (found);

    created = new Channel(name);
    _channels.push_back(created);

    return (created);
}


// Removes a client from every channel it joined.
void    ChannelManager::removeClientFromAll(Client *client)
{
    std::vector<Channel *>::iterator    it;
    Channel                             *channel;

    it = _channels.begin();
    while (it != _channels.end())
    {
        channel = *it;
        channel->members().remove(client);
        channel->operators().remove(client);
        channel->invites().remove(client);

        if (channel->members().isEmpty())
        {
            delete channel;
            it = _channels.erase(it);
        }
        else
            ++it;
    }
}


// Finds a matching stored item.
Channel *ChannelManager::find(const std::string &name)
{
    std::vector<Channel *>::iterator    it;

    it = _channels.begin();
    while (it != _channels.end())
    {
        if ((*it)->name() == name)
            return (*it);
        ++it;
    }

    return (NULL);
}


// Removes empty.
void    ChannelManager::removeEmpty(Channel *channel)
{
    std::vector<Channel *>::iterator    it;

    if (channel == NULL || !channel->members().isEmpty())
        return ;

    it = _channels.begin();
    while (it != _channels.end())
    {
        if (*it == channel)
        {
            delete *it;
            _channels.erase(it);
            return ;
        }
        ++it;
    }
}


// Removes all stored data.
void    ChannelManager::clear()
{
    std::vector<Channel *>::iterator    it;

    it = _channels.begin();
    while (it != _channels.end())
    {
        delete *it;
        ++it;
    }
    _channels.clear();
}
