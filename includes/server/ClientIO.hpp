#ifndef CLIENTIO_HPP
# define CLIENTIO_HPP

// Owns channels and coordinates channel lookup and removal.
class ChannelManager;
// Owns connected clients and provides client lookup operations.
class ClientManager;

// Performs non-blocking receive, send, and removal operations.
class ClientIO
{
public:
    // Initializes this object with the supplied state.
    ClientIO();
    // Destroys this object and releases its owned resources.
    ~ClientIO();

    // Receives available bytes from one client into its receive buffer.
    bool    receive(ClientManager &, ChannelManager &, int);
    // Removes a disconnected client from channels and client storage.
    void    remove(ClientManager &, ChannelManager &, int);
    // Sends pending bytes from one client send buffer.
    void    send(ClientManager &, ChannelManager &, int);

private:
    // Initializes this object with the supplied state.
    ClientIO(const ClientIO &);
    // Performs the &operator= operation.
    ClientIO &operator=(const ClientIO &);
};

#endif
