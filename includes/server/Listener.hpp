#ifndef LISTENER_HPP
# define LISTENER_HPP

// Owns and configures the non-blocking listening socket.
class Listener
{
public:
    // Initializes this object with the supplied state.
    Listener(int);
    // Destroys this object and releases its owned resources.
    ~Listener();

    // Sets up the resources required by this component.
    bool    setup();
    // Accepts one pending connection and configures its client socket.
    bool    acceptClient(int &);
    // Returns the owned socket file descriptor.
    int     fd() const;
    // Performs the close socket operation.
    void    closeSocket();

private:
    int _port;
    int _listenFd;

    // Initializes this object with the supplied state.
    Listener();
    // Initializes this object with the supplied state.
    Listener(const Listener &);
    // Performs the &operator= operation.
    Listener &operator=(const Listener &);

    // Performs the create socket operation.
    bool    createSocket();
    // Updates socket option.
    bool    setSocketOption();
    // Updates non blocking.
    bool    setNonBlocking(int);
    // Performs the bind socket operation.
    bool    bindSocket();
    // Performs the listen socket operation.
    bool    listenSocket();
    // Performs the report system error operation.
    bool    reportSystemError(const char *);
};

#endif
