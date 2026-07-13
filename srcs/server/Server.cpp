#include "server/Server.hpp"
#include "client/Client.hpp"
#include "server/Signal.hpp"

#include <iostream>
#include <string>
#include <unistd.h>


Server::Server(int port, const std::string &password)
    : _password(password),
      _clients(),
      _channels(),
      _listener(port),
      _poll(),
      _clientIO(),
      _message(_password, _clients, _channels),
      _event(),
      _signal()
{
}


Server::~Server()
{
}


// start the server event loop until shutdown is requested
bool    Server::run()
{
    std::vector<struct pollfd>  pollFds;

    if (!_signal.setup())
        return (false);

    if (!_listener.setup())
        return (false);

    while (!_signal.shouldStop())
    {
        _poll.build(pollFds, _listener, _clients);  // add fd to monitor

        if (!_event.wait(pollFds))
            return (false);

        handlePoll(pollFds);
    }

    std::cout << "server shutting down" << std::endl;

    return (true);
}


// routes poll events to terminal, listener, or client handlers
void    Server::handlePoll(std::vector<struct pollfd> pollFds)
{
    size_t index;

    index = 0;
    while (index < pollFds.size() && !_signal.shouldStop())
    {
        if (pollFds[index].revents == 0)    // if no event, go to next index
        {
            ++index;
            continue ;
        }

        if (pollFds[index].fd == STDIN_FILENO)      // command DIE
        {
            if (pollFds[index].revents & POLLIN)
                handleTerminal();
        }
        else if (pollFds[index].fd == _listener.listenFd())  // listening socket
        {
            if (pollFds[index].revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                std::cerr << "Error: listener socket failure" << std::endl;
                _signal.requestStop();
            }
            else if ((pollFds[index].revents & POLLIN) && !acceptClients())
                _signal.requestStop();
        }
        else        // otherwise, all are general clients
            handleClient(pollFds[index].fd, pollFds[index].revents);

        ++index;
    }
}


// Processes read, write, hang-up, and error events for one client.
void    Server::handleClient(int clientFd, short revents)
{
    Client  *client;
    bool    hasReceiveData;

    hasReceiveData = false;

    if (revents & (POLLERR | POLLNVAL))
    {
        _clientIO.remove(_clients, _channels, clientFd);
        return ;
    }

    if ((revents & POLLHUP) && !(revents & POLLIN))
    {
        _clientIO.remove(_clients, _channels, clientFd);
        return ;
    }

    if (revents & POLLIN)
    {
        if (!_clientIO.receive(_clients, _channels, clientFd))
            return ;

        if (_signal.shouldStop())
            return ;

        hasReceiveData = true;
    }

    if (revents & POLLOUT)
    {
        _clientIO.send(_clients, _channels, clientFd);

        if (_signal.shouldStop())
            return ;
    }

    client = _clients.findByFd(clientFd);
    if (hasReceiveData && client != NULL && !_message.process(*client))
    {
        _clientIO.send(_clients, _channels, clientFd);
        _clientIO.remove(_clients, _channels, clientFd);
    }
}


// Processes a command read from standard input.
void    Server::handleTerminal()
{
    std::string line;

    if (!std::getline(std::cin, line))
        return ;

    if (line == "DIE")
        _signal.requestStop();
}


// Accepts one pending client connection without blocking.
bool    Server::acceptClients()
{
    int clientFd;

    if (!_listener.acceptClient(clientFd))
        return (false);

    if (clientFd == -1)
        return (true);

    _clients.add(clientFd);
    std::cout << "client connected with fd " << clientFd << std::endl;

    return (true);
}
