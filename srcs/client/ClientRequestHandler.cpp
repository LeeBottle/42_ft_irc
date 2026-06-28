#include "client/ClientRequestHandler.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "command/Command.hpp"
#include "command/CommandHandlers.hpp"
#include "command/CommandParser.hpp"
#include "command/CommandRouter.hpp"

#include <cerrno>
#include <iostream>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

ClientRequestHandler::ClientRequestHandler()
{
}

ClientRequestHandler::~ClientRequestHandler()
{
}

void    ClientRequestHandler::handleEvent(ClientManager &clients, 
    CommandRouter &router, CommandHandlers &handlers, int clientFd, short revents)
{
    if (revents & (POLLERR | POLLHUP | POLLNVAL))
    {
        clients.remove(clientFd);
        return ;
    }
    if (revents & POLLIN)
        receiveClient(clients, router, handlers, clientFd);
    if (revents & POLLOUT)
        sendToClient(clients, clientFd);
}

void    ClientRequestHandler::receiveClient(ClientManager &clients, 
    CommandRouter &router, CommandHandlers &handlers, int clientFd)
{
    char        buffer[512];
    ssize_t     bytesRead;
    Client      *client;

    client = clients.find(clientFd);
    if (client == NULL)
        return ;
    while (true)
    {
        bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
        if (bytesRead > 0)
        {
            client->appendReceived(buffer, static_cast<size_t>(bytesRead));
            if (!processReceivedLines(router, handlers, *client))
            {
                clients.remove(clientFd);
                return ;
            }
            continue ;
        }
        if (bytesRead == 0)
            clients.remove(clientFd);
        else if (errno == EINTR)
            continue ;
        else if (errno != EAGAIN && errno != EWOULDBLOCK)
            clients.remove(clientFd);
        return ;
    }
}

bool    ClientRequestHandler::processReceivedLines(CommandRouter &router, 
    CommandHandlers &handlers, Client &client)
{
    std::string line;
    Command     command;

    while (client.extractLine(line))
    {
        std::cout << "received from fd " << client.getFd()
            << ": " << line << std::endl;
        if (CommandParser::parse(line, command))
        {
            if (command.getName() == "QUIT")
            {
                send(client.getFd(), ":ircserv ERROR :Closing Link\r\n",
                    sizeof(":ircserv ERROR :Closing Link\r\n") - 1, 0);
                return (false);
            }
            router.route(handlers, client, command);
        }
        command = Command();
    }
    return (true);
}

void    ClientRequestHandler::sendToClient(ClientManager &clients, int clientFd)
{
    Client  *client;
    ssize_t bytesSent;

    client = clients.find(clientFd);
    if (client == NULL)
        return ;
    while (client->hasPendingSend())
    {
        bytesSent = send(clientFd, client->getSendData(),
                client->getSendSize(), 0);
        if (bytesSent > 0)
            client->removeSent(static_cast<size_t>(bytesSent));
        else if (bytesSent == -1 && errno == EINTR)
            continue ;
        else if (bytesSent == -1
            && (errno == EAGAIN || errno == EWOULDBLOCK))
            return ;
        else
        {
            clients.remove(clientFd);
            return ;
        }
    }
}
