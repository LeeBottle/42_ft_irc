#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include "Client.hpp"
# include "MessageHandler.hpp"

# include <poll.h>
# include <string>
# include <vector>

class Connection
{
private:
    int                         _serverFd;
    const std::string           _password;
    std::vector<struct pollfd>  _pollFds;
    std::vector<Client *>       _clients;
    MessageHandler        _messageHandler;

public:
    Connection(int, const std::string &);
    ~Connection();

    void    run();
    Client  *findClient(int);
    const std::string   &getPassword() const;
    const std::vector<Client *> &getClients() const;
    void    enableClientWrite(int);
    void    disableClientWrite(int);
    void    disconnectClient(int);

private:
    Connection();
    Connection(const Connection &);
    Connection& operator=(const Connection &);

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
