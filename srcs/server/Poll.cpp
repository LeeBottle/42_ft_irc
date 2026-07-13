#include "server/Poll.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "server/Listener.hpp"

#include <unistd.h>


// Initializes this object with the supplied state.
Poll::Poll()
{
}


// Destroys this object and releases its owned resources.
Poll::~Poll()
{
}


// Rebuilds the poll descriptor list from the listener and connected clients.
void    Poll::build(std::vector<struct pollfd> &pollFds, Listener &listener,
    ClientManager &clients) const
{
    pollFds.clear();
    appendTerminal(pollFds);
    appendListener(pollFds, listener);
    appendClients(pollFds, clients);
}


// Performs the append terminal operation.
void    Poll::appendTerminal(std::vector<struct pollfd> &pollFds) const
{
    struct pollfd pollFd;

    if (!isatty(STDIN_FILENO))  // allowed only terminal
        return ;

    pollFd.fd = STDIN_FILENO;
    pollFd.events = POLLIN;
    pollFd.revents = 0;
    pollFds.push_back(pollFd);
}


// Performs the append listener operation.
void    Poll::appendListener(std::vector<struct pollfd> &pollFds,
    Listener &listener) const
{
    struct pollfd pollFd;

    pollFd.fd = listener.fd();
    pollFd.events = POLLIN;  // request new client connection in accept queue
    pollFd.revents = 0;
    pollFds.push_back(pollFd);
}

// Performs the append clients operation.
void    Poll::appendClients(std::vector<struct pollfd> &pollFds,
    ClientManager &clientManager) const
{
    std::vector<Client *>::const_iterator   it;
    const std::vector<Client *>             &clients = clientManager.clients();
    struct pollfd                           pollFd;

    it = clients.begin();
    while (it != clients.end())
    {
        pollFd.fd = (*it)->fd();
        pollFd.events = POLLIN;

        if ((*it)->sendBuffer().hasData())
            pollFd.events |= POLLOUT;   // pollFd.events = POLLIN | POLLOUT

        pollFd.revents = 0;
        pollFds.push_back(pollFd);
        ++it;
    }
}
