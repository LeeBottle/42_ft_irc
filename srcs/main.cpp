#include <iostream>
#include <string>
#include "Server.hpp"

int parsePort(std::string inputPort)
{
    int         port = 0;
    std::size_t i = 0;

    if (inputPort.empty())
        return (-1);

    while (i < inputPort.size())
    {
        if (inputPort[i] < '0' || inputPort[i] > '9')
            return (-1);

        port = port * 10 + (inputPort[i] - '0');
        if (port > 65535)
            return (-1);
        
        ++i;
    }

    if (port == 0)  // Port 0 is used that kernel allocates ramdom port to client object
        return (-1);

    return (port);
}

bool    validatePassword(const std::string &password)
{
    if (password.empty() || password.size() > 504)  // size() count based on bytes
        return (false);

    if (password.find('\0') != std::string::npos
        || password.find('\r') != std::string::npos
        || password.find('\n') != std::string::npos)
        return (false);

    return (true);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return (1);
    }

    const std::string inputPort(argv[1]);
    const std::string password(argv[2]);
    const int         port = parsePort(inputPort);

    if (port == -1)
    {
        std::cerr << "Error: invalid port" << std::endl;
        return (-1);
    }

    if (!validatePassword(password))
    {
        std::cerr << "Error: invalid password" << std::endl;
        return (-1);
    }
    
    Server server(port, password);

    server.run();

    return (0);
}
