#include "Connection.hpp"

#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>

Connection::Connection(int serverFd, const std::string &password)
    : _serverFd(serverFd), _password(password), _pollFds(), _clients(),
    _messageHandler()
{
    addPollFd(_serverFd, POLLIN);
}

Connection::~Connection()
{
    closeClients();
}

void    Connection::run()
{
    pollEvents();
}

void    Connection::pollEvents()
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

void    Connection::acceptClients()
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
    }
}

void    Connection::addClient(int clientFd)
{
    Client *client;

    client = new Client(clientFd);
    _clients.push_back(client);
    addPollFd(clientFd, POLLIN);
}

void    Connection::handleClientEvent(int clientFd, short revents)
{
    if (revents & POLLIN)
        _messageHandler.receiveClient(*this, clientFd);
    if (findClient(clientFd) == NULL)
        return ;
    if (revents & POLLOUT)
        _messageHandler.sendToClient(*this, clientFd);
    if (findClient(clientFd) == NULL)
        return ;
    if (revents & (POLLERR | POLLHUP | POLLNVAL))
    {
        disconnectClient(clientFd);
    }
}

void    Connection::setNonBlocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        exitWithError("fcntl");
}

void    Connection::addPollFd(int fd, short events)
{
    struct pollfd pollFd;

    pollFd.fd = fd;
    pollFd.events = events;
    pollFd.revents = 0;
    _pollFds.push_back(pollFd);
}

void    Connection::removePollFd(int fd)
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

void    Connection::updatePollFd(int fd, short events)
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

Client  *Connection::findClient(int clientFd)
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

const std::vector<Client *> &Connection::getClients() const
{
    return (_clients);
}

const std::string   &Connection::getPassword() const
{
    return (_password);
}

void    Connection::enableClientWrite(int clientFd)
{
    updatePollFd(clientFd, POLLIN | POLLOUT);
}

void    Connection::disableClientWrite(int clientFd)
{
    updatePollFd(clientFd, POLLIN);
}

void    Connection::disconnectClient(int clientFd)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if ((*it)->getFd() == clientFd)
        {
            _messageHandler.removeClientFromChannels(**it);
            delete *it;
            _clients.erase(it);
            removePollFd(clientFd);
            return ;
        }
        ++it;
    }
}

void    Connection::closeClients()
{
    while (!_clients.empty())
        disconnectClient(_clients.back()->getFd());
}

void    Connection::exitWithError(const char *functionName)
{
    std::cerr << functionName << ": " << std::strerror(errno) << std::endl;
    std::exit(EXIT_FAILURE);
}
