#include "client/ClientManager.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"

ClientManager::ClientManager(ChannelManager &channels)
    : _clients(), _channels(channels)
{
}

ClientManager::~ClientManager()
{
    closeAll();
}

Client  *ClientManager::add(int clientFd)
{
    Client *client;

    client = new Client(clientFd);
    _clients.push_back(client);
    return (client);
}

Client  *ClientManager::find(int clientFd)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if ((*it)->getFd() == clientFd)
            return (*it);
        ++it;
    }
    return (NULL);
}

Client  *ClientManager::findByNickname(const std::string &nickname)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if ((*it)->getNickname() == nickname)
            return (*it);
        ++it;
    }
    return (NULL);
}

void    ClientManager::remove(int clientFd)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if ((*it)->getFd() == clientFd)
        {
            _channels.removeClientFromAll(*it);
            delete *it;
            _clients.erase(it);
            return ;
        }
        ++it;
    }
}

void    ClientManager::closeAll()
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        _channels.removeClientFromAll(*it);
        delete *it;
        ++it;
    }
    _clients.clear();
}

void    ClientManager::appendPollFds(std::vector<struct pollfd> &pollFds) const
{
    std::vector<Client *>::const_iterator   it;
    struct pollfd                           pollFd;

    it = _clients.begin();
    while (it != _clients.end())
    {
        pollFd.fd = (*it)->getFd();
        pollFd.events = POLLIN;
        if ((*it)->hasPendingSend())
            pollFd.events |= POLLOUT;
        pollFd.revents = 0;
        pollFds.push_back(pollFd);
        ++it;
    }
}

bool    ClientManager::isNicknameInUse(const std::string &nickname,
        Client &owner) const
{
    std::vector<Client *>::const_iterator   it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if (*it != &owner && (*it)->getNickname() == nickname)
            return (true);
        ++it;
    }
    return (false);
}
