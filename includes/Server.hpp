#ifndef SERVER_HPP
# define SERVER_HPP

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
    void    setNonBlocking();
    void    bindSocket();
    void    listenSocket();
    void    addPollFd(int, short);
    void    pollEvents();
    void    acceptClient();
    void    closeSocket();
    void    exitWithError(const char *);
};

#endif
