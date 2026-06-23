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
    closeSocket();
}

void    Server::run()
{
    createSocket();
    setSocketOption();
    setNonBlocking();
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

void    Server::setNonBlocking()
{
    if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == -1)
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

void    Server::addPollFd(int fd, short events)
{
    struct pollfd   pollFd;

    pollFd.fd = fd;
    pollFd.events = events;
    pollFd.revents = 0;
    _pollFds.push_back(pollFd);
}

void    Server::pollEvents()
{
    int         readyCount;

    while (true)
    {
        readyCount = poll(&_pollFds[0], _pollFds.size(), -1);
        if (readyCount == -1)
        {
            if (errno == EINTR)
                continue;
            exitWithError("poll");
        }
        if (_pollFds[0].revents & POLLIN)
            acceptClient();
    }
}

void    Server::acceptClient()
{
    int clientFd;

    while (true)
    {
        clientFd = accept(_serverFd, NULL, NULL);
        if (clientFd != -1)
            break;
        if (errno == EINTR)
            continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        exitWithError("accept");
    }
    std::cout << "client connected with fd " << clientFd << std::endl;
    close(clientFd);
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

