#ifndef SERVER_HPP
# define SERVER_HPP

# include <poll.h>
# include <string>
# include <vector>

# include "channel/ChannelManager.hpp"
# include "client/ClientManager.hpp"
# include "client/ClientPollEventHandler.hpp"
# include "server/ServerMessageSwitch.hpp"
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
    ServerMessageSwitch     _messageSwitch;
    ClientPollEventHandler  _clientPollEventHandler;

    Server();
    Server(const Server &);
    Server &operator=(const Server &);

    bool    setupSignalHandler();
    bool    runEventLoop();
    bool    acceptPendingClients();
    bool    processReceivedMessages(Client &);
    void    appendTerminalPollFd(std::vector<struct pollfd> &) const;
    void    buildPollFds(std::vector<struct pollfd> &) const;
    void    handlePollEvents(std::vector<struct pollfd> &);
    void    handleTerminalInput();
    bool    shouldStop() const;
    bool    reportSystemError(const char *);
};

#endif
