#ifndef SERVER_CONNECTION_HPP
# define SERVER_CONNECTION_HPP

# include "Client.hpp"
# include "ServerMessageHandler.hpp"

# include <poll.h>
# include <vector>

class ServerConnection
{
private:
    int                         _serverFd;
    std::vector<struct pollfd>  _pollFds;
    std::vector<Client *>       _clients;
    ServerMessageHandler        _messageHandler;

public:
    ServerConnection(int);
    ~ServerConnection();

    void    run();
    Client  *findClient(int);
    const std::vector<Client *> &getClients() const;
    void    enableClientWrite(int);
    void    disableClientWrite(int);
    void    disconnectClient(int);

private:
    ServerConnection();
    ServerConnection(const ServerConnection &);
    ServerConnection& operator=(const ServerConnection &);

    void    pollEvents();
    void    acceptClients();
    void    addClient(int);
    void    handleClientEvent(int, short);
    void    setNonBlocking(int);
    void    addPollFd(int, short);
    void    removePollFd(int);
    void    updatePollFd(int, short);
    void    closeClients();
    void    exitWithError(const char *);
};

#endif
