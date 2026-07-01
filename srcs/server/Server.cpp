#include "server/Server.hpp"
#include "client/Client.hpp"
#include "parser/Parser.hpp"
#include "parser/Parser.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <poll.h>
#include <signal.h>
#include <string>
#include <unistd.h>

static volatile sig_atomic_t   g_stopRequested = 0;

static void handleStopSignal(int signalNumber)
{
    (void)signalNumber;
    g_stopRequested = 1;
}

Server::Server(int port, const std::string &password)
    : _password(password), _channels(), _socket(port), _clients(_channels),
      _messageSwitch(_password, _clients, _channels), _clientPollEventHandler()
{
}

Server::~Server()
{
}

bool    Server::run()
{
    if (!setupSignalHandler())
        return (false);
    if (!_socket.setup())
        return (false);
    return (runEventLoop());
}

bool    Server::setupSignalHandler()
{
    struct sigaction action;
    struct sigaction ignoreAction;

    g_stopRequested = 0;
    std::memset(&action, 0, sizeof(action));
    action.sa_handler = handleStopSignal;
    if (sigemptyset(&action.sa_mask) == -1)
        return (reportSystemError("sigemptyset"));
    if (sigaction(SIGINT, &action, NULL) == -1)
        return (reportSystemError("sigaction"));
    if (sigaction(SIGTERM, &action, NULL) == -1)
        return (reportSystemError("sigaction"));
    std::memset(&ignoreAction, 0, sizeof(ignoreAction));
    ignoreAction.sa_handler = SIG_IGN;
    if (sigemptyset(&ignoreAction.sa_mask) == -1)
        return (reportSystemError("sigemptyset"));
    if (sigaction(SIGPIPE, &ignoreAction, NULL) == -1)
        return (reportSystemError("sigaction"));
    return (true);
}

bool    Server::runEventLoop()
{
    std::vector<struct pollfd>  pollFds;
    int                         readyCount;

    while (!shouldStop())
    {
        buildPollFds(pollFds);
        readyCount = poll(&pollFds[0], pollFds.size(), -1);
        if (readyCount == -1)
        {
            if (errno == EINTR)
                continue ;
            return (reportSystemError("poll"));
        }
        handlePollEvents(pollFds);
    }
    std::cout << "server shutting down" << std::endl;
    return (true);
}

void    Server::buildPollFds(std::vector<struct pollfd> &pollFds) const
{
    struct pollfd pollFd;

    pollFds.clear();
    appendTerminalPollFd(pollFds);
    pollFd.fd = _socket.getFd();
    pollFd.events = POLLIN;
    pollFd.revents = 0;
    pollFds.push_back(pollFd);
    _clients.appendPollFds(pollFds);
}

void    Server::appendTerminalPollFd(std::vector<struct pollfd> &pollFds) const
{
    struct pollfd pollFd;

    if (!isatty(STDIN_FILENO))
        return ;
    pollFd.fd = STDIN_FILENO;
    pollFd.events = POLLIN;
    pollFd.revents = 0;
    pollFds.push_back(pollFd);
}

void    Server::handlePollEvents(std::vector<struct pollfd> &pollFds)
{
    size_t index;
    int    fd;
    short  revents;
    bool   receivedData;
    Client *client;

    index = pollFds.size();
    while (index > 0 && !shouldStop())
    {
        --index;
        fd = pollFds[index].fd;
        revents = pollFds[index].revents;
        if (revents == 0)
            continue ;
        if (fd == STDIN_FILENO)
        {
            if (revents & POLLIN)
                handleTerminalInput();
        }
        else if (fd == _socket.getFd() && (revents & POLLIN))
            acceptPendingClients();
        else if (fd != _socket.getFd())
        {
            if (!_clientPollEventHandler.handleEvent(_clients, fd, revents,
                    receivedData))
                continue ;
            client = _clients.find(fd);
            if (receivedData && client != NULL
                && !processReceivedMessages(*client))
                _clients.remove(fd);
        }
    }
}

void    Server::handleTerminalInput()
{
    std::string line;

    if (!std::getline(std::cin, line))
        return ;
    if (line == "DIE")
        g_stopRequested = 1;
}

bool    Server::processReceivedMessages(Client &client)
{
    Parser   message;
    std::string     line;

    while (Parser::popLine(client, line))
    {
        std::cout << "received from fd " << client.getFd()
            << ": " << line << std::endl;
        if (Parser::parse(line, message))
        {
            if (!_messageSwitch.branch(client, message))
                return (false);
        }
        message = Parser();
    }
    return (true);
}

bool    Server::acceptPendingClients()
{
    int clientFd;

    while (true)
    {
        if (!_socket.acceptClient(clientFd))
            return (false);
        if (clientFd == -1)
            return (true);
        _clients.add(clientFd);
        std::cout << "client connected with fd " << clientFd << std::endl;
    }
}

bool    Server::shouldStop() const
{
    return (g_stopRequested != 0);
}

bool    Server::reportSystemError(const char *functionName)
{
    std::cerr << functionName << ": " << std::strerror(errno) << std::endl;
    return (false);
}
