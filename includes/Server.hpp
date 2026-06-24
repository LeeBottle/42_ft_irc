#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>

class Server
{
private:
    const int           _port;
    const std::string   _password;
    int                 _serverFd;

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
    void    closeSocket();
    void    exitWithError(const char *);
};

#endif
