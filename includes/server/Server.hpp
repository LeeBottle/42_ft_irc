#ifndef SERVER_HPP
# define SERVER_HPP

# include <poll.h>
# include <string>
# include <vector>

# include "channel/ChannelManager.hpp"
# include "client/ClientManager.hpp"
# include "client/ClientRequestHandler.hpp"
# include "command/CommandHandlers.hpp"
# include "command/CommandRouter.hpp"
# include "server/ServerSocket.hpp"

class Server
{
public:
    Server(int, const std::string &);
    ~Server();

    bool    run();

private:
    std::string             _password;
    ChannelManager          _channels;
    ServerSocket            _socket;
    ClientManager           _clients;
    CommandRouter           _commandRouter;
    CommandHandlers         _commandHandlers;
    ClientRequestHandler    _clientRequestHandler;

    Server();
    Server(const Server &);
    Server &operator=(const Server &);

    bool    setupSignalHandler();
    bool    runEventLoop();
    bool    acceptPendingClients();
    void    buildPollFds(std::vector<struct pollfd> &) const;
    void    handlePollEvents(std::vector<struct pollfd> &);
    bool    shouldStop() const;
    bool    reportSystemError(const char *);
};

#endif
