#include "Server.hpp"
#include "ServerConnection.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int port, const std::string& password)
    : _port(port), _password(password), _serverFd(-1)
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
    setNonBlocking(_serverFd);
    bindSocket();
    listenSocket();
    std::cout << "server is listening on port " << _port << std::endl;
    ServerConnection connection(_serverFd, _password);

    connection.run();
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
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &option,
            sizeof(option)) == -1)
        exitWithError("setsockopt");
}

void    Server::setNonBlocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        exitWithError("fcntl");
}

void    Server::bindSocket()
{
    struct sockaddr_in address;

    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(_port);
    if (bind(_serverFd, reinterpret_cast<struct sockaddr *>(&address),
            sizeof(address)) == -1)
        exitWithError("bind");
}

void    Server::listenSocket()
{
    if (listen(_serverFd, SOMAXCONN) == -1)
        exitWithError("listen");
}

void    Server::closeSocket()
{
    if (_serverFd != -1)
    {
        close(_serverFd);
        _serverFd = -1;
    }
}

void    Server::exitWithError(const char *functionName)
{
    std::cerr << functionName << ": " << std::strerror(errno) << std::endl;
    closeSocket();
    std::exit(EXIT_FAILURE);
}
