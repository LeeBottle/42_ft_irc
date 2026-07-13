#ifndef POLL_HPP
# define POLL_HPP

# include <poll.h>
# include <vector>

// Owns connected clients and provides client lookup operations.
class ClientManager;
// Owns and configures the non-blocking listening socket.
class Listener;

// Builds the single poll descriptor list used by the server.
class Poll
{
public:
    // Initializes this object with the supplied state.
    Poll();
    // Destroys this object and releases its owned resources.
    ~Poll();

    // Rebuilds the poll descriptor list from the listener and connected clients.
    void    build(std::vector<struct pollfd> &, Listener &, 
                ClientManager &) const;

private:
    // Initializes this object with the supplied state.
    Poll(const Poll &);
    // Performs the &operator= operation.
    Poll &operator=(const Poll &);

    // Performs the append clients operation.
    void    appendClients(std::vector<struct pollfd> &, ClientManager &) const;
    // Performs the append listener operation.
    void    appendListener(std::vector<struct pollfd> &, Listener &) const;
    // Performs the append terminal operation.
    void    appendTerminal(std::vector<struct pollfd> &) const;
};

#endif
