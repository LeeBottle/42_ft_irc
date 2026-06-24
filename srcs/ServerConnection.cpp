#include "ServerConnection.hpp"

#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>

ServerConnection::ServerConnection(int serverFd)
    : _serverFd(serverFd), _pollFds(), _clients(), _messageHandler(*this)
{
    addPollFd(_serverFd, POLLIN);
}

ServerConnection::~ServerConnection()
{
    closeClients();
}

void    ServerConnection::run()
{
    pollEvents();
}

void    ServerConnection::pollEvents()
{
    int         readyCount;
    std::size_t index;
    int         fd;
    short       revents;

    while (true)
    {
        readyCount = poll(&_pollFds[0], _pollFds.size(), -1);
        if (readyCount == -1)
        {
            if (errno == EINTR)
                continue ;
            exitWithError("poll");
        }
        index = _pollFds.size();
        while (index > 0)
        {
            --index;
            fd = _pollFds[index].fd;
            revents = _pollFds[index].revents;
            if (revents == 0)
                continue ;
            if (fd == _serverFd && (revents & POLLIN))
                acceptClients();
            else if (fd != _serverFd)
                handleClientEvent(fd, revents);
        }
    }
}

void    ServerConnection::acceptClients()
{
    int clientFd;

    while (true)
    {
        clientFd = accept(_serverFd, NULL, NULL);
        if (clientFd == -1)
        {
            if (errno == EINTR)
                continue ;
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return ;
            exitWithError("accept");
        }
        setNonBlocking(clientFd);
        addClient(clientFd);
        std::cout << "client connected with fd " << clientFd << std::endl;
    }
}

void    ServerConnection::addClient(int clientFd)
{
    Client *client;

    client = new Client(clientFd);
    _clients.push_back(client);
    addPollFd(clientFd, POLLIN);
}

void    ServerConnection::handleClientEvent(int clientFd, short revents)
{
    if (revents & POLLIN)
        _messageHandler.receiveClient(clientFd);
    if (findClient(clientFd) == NULL)
        return ;
    if (revents & POLLOUT)
        _messageHandler.sendToClient(clientFd);
    if (findClient(clientFd) == NULL)
        return ;
    if (revents & (POLLERR | POLLHUP | POLLNVAL))
    {
        std::cout << "client disconnected with fd " << clientFd << std::endl;
        disconnectClient(clientFd);
    }
}

void    ServerConnection::setNonBlocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        exitWithError("fcntl");
}

void    ServerConnection::addPollFd(int fd, short events)
{
    struct pollfd pollFd;

    pollFd.fd = fd;
    pollFd.events = events;
    pollFd.revents = 0;
    _pollFds.push_back(pollFd);
}

void    ServerConnection::removePollFd(int fd)
{
    std::vector<struct pollfd>::iterator it;

    it = _pollFds.begin();
    while (it != _pollFds.end())
    {
        if (it->fd == fd)
        {
            _pollFds.erase(it);
            return ;
        }
        ++it;
    }
}

void    ServerConnection::updatePollFd(int fd, short events)
{
    std::vector<struct pollfd>::iterator it;

    it = _pollFds.begin();
    while (it != _pollFds.end())
    {
        if (it->fd == fd)
        {
            it->events = events;
            return ;
        }
        ++it;
    }
}

Client  *ServerConnection::findClient(int clientFd)
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

const std::vector<Client *> &ServerConnection::getClients() const
{
    return (_clients);
}

void    ServerConnection::enableClientWrite(int clientFd)
{
    updatePollFd(clientFd, POLLIN | POLLOUT);
}

void    ServerConnection::disableClientWrite(int clientFd)
{
    updatePollFd(clientFd, POLLIN);
}

void    ServerConnection::disconnectClient(int clientFd)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if ((*it)->getFd() == clientFd)
        {
            delete *it;
            _clients.erase(it);
            removePollFd(clientFd);
            return ;
        }
        ++it;
    }
}

void    ServerConnection::closeClients()
{
    while (!_clients.empty())
        disconnectClient(_clients.back()->getFd());
}

void    ServerConnection::exitWithError(const char *functionName)
{
    std::cerr << functionName << ": " << std::strerror(errno) << std::endl;
    std::exit(EXIT_FAILURE);
}
