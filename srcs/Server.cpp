#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>

Server::Server(int port, const std::string& password)
    : _port(port),
      _password(password),
      _serverFd(-1)
{
}

Server::~Server()
{
    closeClients();
    closeSocket();
}

void    Server::run()
{
    createSocket();
    setSocketOption();
    setNonBlocking(_serverFd);
    bindSocket();
    listenSocket();
    addPollFd(_serverFd, POLLIN);
    std::cout << "server is listening on port " << _port << std::endl;
    pollEvents();
}

void    Server::createSocket()
{
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd == -1)
        exitWithError("socket");
}

void    Server::setSocketOption()
{
    int option;

    option = 1;
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
        exitWithError("setsockopt");
}

void    Server::setNonBlocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        exitWithError("fcntl");
}

void    Server::bindSocket()
{
    struct sockaddr_in  address;

    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(_port);
    if (bind(_serverFd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) == -1)
        exitWithError("bind");
}

void    Server::listenSocket()
{
    if (listen(_serverFd, SOMAXCONN) == -1)
        exitWithError("listen");
}

void    Server::pollEvents()
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
                acceptClient();
            else if (fd != _serverFd)
                handleClientEvent(fd, revents);
        }
    }
}

void    Server::acceptClient()
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

void    Server::addClient(int clientFd)
{
    Client  *client;

    client = new Client(clientFd);
    _clients.push_back(client);
    addPollFd(clientFd, POLLIN);
}

void    Server::handleClientEvent(int clientFd, short revents)
{
    if (revents & POLLIN)
        receiveClient(clientFd);
    else if (revents & (POLLERR | POLLHUP | POLLNVAL))
        removeClient(clientFd);
}

void    Server::receiveClient(int clientFd)
{
    char    buffer[512];
    ssize_t received;
    Client  *client;

    client = findClient(clientFd);
    if (client == NULL)
        return ;
    while (true)
    {
        received = recv(clientFd, buffer, sizeof(buffer), 0);
        if (received > 0)
        {
            client->appendReceived(buffer, static_cast<std::string::size_type>(received));
            continue ;
        }
        if (received == 0)
            removeClient(clientFd);
        else if (errno == EINTR)
            continue ;
        else if (errno != EAGAIN && errno != EWOULDBLOCK)
            removeClient(clientFd);
        
        return ;
    }
}

Client  *Server::findClient(int clientFd)
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

void    Server::removeClient(int clientFd)
{
    std::vector<Client *>::iterator it;

    it = _clients.begin();
    while (it != _clients.end())
    {
        if ((*it)->getFd() == clientFd)
        {
            delete  *it;
            _clients.erase(it);
            removePollFd(clientFd);
            return ;
        }
        ++it;
    }
}

void    Server::addPollFd(int fd, short events)
{
    struct pollfd   pollFd;

    pollFd.fd = fd;
    pollFd.events = events;
    pollFd.revents = 0;
    _pollFds.push_back(pollFd);
}

void    Server::removePollFd(int fd)
{
    std::vector<struct pollfd>::iterator    it;

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

void    Server::closeClients()
{
    while (!_clients.empty())
        removeClient(_clients.back()->getFd());
}

void    Server::closeSocket()
{
    if (_serverFd != -1)
    {
        close(_serverFd);
        _serverFd = -1;
    }
}

void    Server::exitWithError(const char *funcName)
{
    std::cerr << funcName << ": " << std::strerror(errno) << std::endl;
    closeSocket();
    std::exit(EXIT_FAILURE);
}

