#ifndef LISTENER_HPP
# define LISTENER_HPP


class Listener
{
public:
    Listener(int);
    ~Listener();

    bool    setup();
    bool    acceptClient(int &);
    int     listenFd() const;
    void    closeSocket();

private:
    int _port;
    int _listenFd;

    Listener();
    Listener(const Listener &);
    Listener &operator=(const Listener &);

    bool    createSocket();
    bool    setSocketOption();
    bool    setNonBlocking(int);
    bool    bindSocket();
    bool    listenSocket();
    bool    reportSystemError(const char *);
};

#endif
