#include "server/ServerSocket.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

ServerSocket::ServerSocket(int port)
    : _port(port), _listenFd(-1)
{
}

ServerSocket::~ServerSocket()
{
    closeSocket();
}

bool    ServerSocket::setup()
{
    if (!createSocket())
        return (false);
    if (!setSocketOption())
        return (false);
    if (!setNonBlocking())
        return (false);
    if (!bindSocket())
        return (false);
    if (!listenSocket())
        return (false);
    return (true);
}

bool    ServerSocket::createSocket()
{
    _listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenFd == -1)
        return (reportSystemError("socket"));
    return (true);
}

bool    ServerSocket::setSocketOption()
{
    int option;

    option = 1;
    if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &option,
            sizeof(option)) == -1)
        return (reportSystemError("setsockopt"));
    return (true);
}

bool    ServerSocket::setNonBlocking()
{
    return (setNonBlocking(_listenFd));
}

bool    ServerSocket::setNonBlocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        return (reportSystemError("fcntl"));
    return (true);
}

bool    ServerSocket::bindSocket()
{
    struct sockaddr_in address;

    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(_port);
    if (bind(_listenFd, reinterpret_cast<struct sockaddr *>(&address),
            sizeof(address)) == -1)
        return (reportSystemError("bind"));
    return (true);
}

bool    ServerSocket::listenSocket()
{
    if (listen(_listenFd, SOMAXCONN) == -1)
        return (reportSystemError("listen"));
    std::cout << "server is listening on port " << _port << std::endl;
    return (true);
}

bool    ServerSocket::acceptClient(int &clientFd)
{
    clientFd = accept(_listenFd, NULL, NULL);
    if (clientFd == -1)
    {
        if (errno == EINTR)
            return (true);
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            clientFd = -1;
            return (true);
        }
        return (reportSystemError("accept"));
    }
    if (!setNonBlocking(clientFd))
    {
        close(clientFd);
        clientFd = -1;
        return (false);
    }
    return (true);
}

int ServerSocket::getFd() const
{
    return (_listenFd);
}

void    ServerSocket::closeSocket()
{
    if (_listenFd != -1)
    {
        close(_listenFd);
        _listenFd = -1;
    }
}

bool    ServerSocket::reportSystemError(const char *functionName)
{
    std::cerr << functionName << ": " << std::strerror(errno) << std::endl;
    return (false);
}
