#include "channel/ChannelManager.hpp"
#include "channel/Channel.hpp"

ChannelManager::ChannelManager()
    : _channels()
{
}

ChannelManager::~ChannelManager()
{
    clearAllChannels();
}

Channel *ChannelManager::findOrCreate(const std::string &name)
{
    Channel *found;
    Channel *created;

    found = findChannel(name);
    if (found != NULL)
        return (found);

    created = new Channel(name);
    _channels.push_back(created);
    return (created);
}

void    ChannelManager::removeClientFromAllChannels(Client *client)
{
    std::vector<Channel *>::iterator it;
    Channel                          *channel;

    it = _channels.begin();
    while (it != _channels.end())
    {
        channel = *it;
        channel->removeClient(client);
        if (channel->isEmpty())
        {
            delete channel;
            it = _channels.erase(it);
        }
        else
            ++it;
    }
}

Channel *ChannelManager::findChannel(const std::string &name)
{
    std::vector<Channel *>::iterator it;

    it = _channels.begin();
    while (it != _channels.end())
    {
        if ((*it)->getName() == name)
            return (*it);
        ++it;
    }
    return (NULL);
}

void    ChannelManager::removeEmptyChannel(Channel *channel)
{
    std::vector<Channel *>::iterator it;

    if (channel == NULL || !channel->isEmpty())
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

void    ChannelManager::clearAllChannels()
{
    std::vector<Channel *>::iterator it;

    it = _channels.begin();
    while (it != _channels.end())
    {
        delete *it;
        ++it;
    }
    _channels.clear();
}
