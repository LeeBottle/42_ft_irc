#include "client/ClientManager.hpp"
#include "client/Client.hpp"


// Initializes this object with the supplied state.
ClientManager::ClientManager() : _clients()
{
}


// Destroys this object and releases its owned resources.
ClientManager::~ClientManager()
{
    clear();
}


// Adds an item to this collection.
Client  *ClientManager::add(int clientFd)
{
    Client  *client;

    client = new Client(clientFd);
    _clients.push_back(client);

    return (client);
}


// Finds a client by its file descriptor.
Client  *ClientManager::findByFd(int clientFd)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if ((*it)->fd() == clientFd)
            return (*it);
        ++it;
    }

    return (NULL);
}


// Finds a client by its nickname.
Client  *ClientManager::findByNickname(const std::string &nickname)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if ((*it)->nickname() == nickname)
            return (*it);
        ++it;
    }

    return (NULL);
}


// Performs the clients operation.
const std::vector<Client *> &ClientManager::clients() const
{
    return (_clients);
}


// Removes and destroys a client identified by its file descriptor.
void    ClientManager::removeByFd(int clientFd)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if ((*it)->fd() == clientFd)
        {
            delete *it;
            _clients.erase(it);
            return ;
        }
        ++it;
    }
}


// Removes all stored data.
void    ClientManager::clear()
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        delete *it;
        ++it;
    }
    _clients.clear();
}


// Reports whether nickname in use.
bool    ClientManager::isNicknameInUse(const std::string &nickname,
    Client &owner) const
{
    std::vector<Client *>::const_iterator   it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if (*it != &owner && (*it)->nickname() == nickname)
            return (true);
        ++it;
    }

    return (false);
}
