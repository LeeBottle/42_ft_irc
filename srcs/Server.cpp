#include "Server.hpp"
#include <iostream>

Server::Server(int port, const std::string& password)
    : _port(port),
      _password(password)
{
}

Server::~Server()
{
}

void    Server::run()
{
    std::cout << "server started" << std::endl;
}
