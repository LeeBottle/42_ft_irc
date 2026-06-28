#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

class ServerSocket
{
public:
    ServerSocket(int);
    ~ServerSocket();

    bool    setup();
    bool    acceptClient(int &);
    int     getFd() const;
    void    closeSocket();

private:
    int _port;
    int _listenFd;

    ServerSocket();
    ServerSocket(const ServerSocket &);
    ServerSocket &operator=(const ServerSocket &);

    bool    createSocket();
    bool    setSocketOption();
    bool    setNonBlocking();
    bool    setNonBlocking(int);
    bool    bindSocket();
    bool    listenSocket();
    bool    reportSystemError(const char *);
};

#endif
