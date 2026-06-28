#include "channel/ChannelManager.hpp"
#include "channel/Channel.hpp"

ChannelManager::ChannelManager()
    : _channels()
{
}

ChannelManager::~ChannelManager()
{
    closeAll();
}

Channel *ChannelManager::getOrCreate(const std::string &name)
{
    Channel *channel;

    channel = find(name);
    if (channel != NULL)
        return (channel);
    channel = new Channel(name);
    _channels.push_back(channel);
    return (channel);
}

void    ChannelManager::removeClientFromAll(Client *client)
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

Channel *ChannelManager::find(const std::string &name)
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

void    ChannelManager::deleteIfEmpty(Channel *channel)
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

void    ChannelManager::closeAll()
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
