#ifndef SERVER_HPP
# define SERVER_HPP

#include "Client.hpp"

# include <string>
# include <vector>
# include <poll.h>

class Server
{
private:
    const int                   _port;
    const std::string           _password;
    int                         _serverFd;
    std::vector<struct pollfd>  _pollFds;
    std::vector<Client *>       _clients;

public:
    Server(int, const std::string &);
    ~Server();

    void    run();

private:
    Server();
    Server(const Server &);
    Server& operator=(const Server &);

    void    createSocket();
    void    setSocketOption();
    void    setNonBlocking(int);
    void    bindSocket();
    void    listenSocket();
    void    addPollFd(int, short);
    void    removePollFd(int);
    void    pollEvents();
    void    acceptClient();
    void    addClient(int);
    void    handleClientEvent(int, short);
    void    receiveClient(int);
    Client  *findClient(int);
    void    closeClients();
    void    removeClient(int);
    void    closeSocket();
    void    exitWithError(const char *);
};

#endif
