#include "ServerMessageHandler.hpp"
#include "ServerConnection.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>

ServerMessageHandler::ServerMessageHandler(ServerConnection &connection)
    : _connection(connection)
{
}

void    ServerMessageHandler::receiveClient(int clientFd)
{
    char    buffer[512];
    ssize_t received;
    Client  *client;

    client = _connection.findClient(clientFd);
    if (client == NULL)
        return ;
    while (true)
    {
        received = recv(clientFd, buffer, sizeof(buffer), 0);
        if (received > 0)
        {
            client->appendReceived(buffer,
                static_cast<std::string::size_type>(received));
            std::cout << "received " << received << " bytes from fd "
                << clientFd << std::endl;
            processReceivedLines(*client);
            continue ;
        }
        if (received == 0)
        {
            std::cout << "client disconnected with fd " << clientFd << std::endl;
            _connection.disconnectClient(clientFd);
        }
        else if (errno == EINTR)
            continue ;
        else if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            std::cerr << "recv: " << std::strerror(errno) << std::endl;
            _connection.disconnectClient(clientFd);
        }
        return ;
    }
}

void    ServerMessageHandler::processReceivedLines(Client &client)
{
    std::string line;
    Message     message;

    while (client.extractLine(line))
    {
        if (!message.parse(line))
        {
            std::cout << "invalid message from fd " << client.getFd()
                << std::endl;
            continue ;
        }
        std::cout << "parsed message from fd " << client.getFd()
            << ": " << message.getRaw() << std::endl;
        broadcastMessage(client.getFd(), message);
    }
}

void    ServerMessageHandler::broadcastMessage(int senderFd,
    const Message &message)
{
    std::vector<Client *>::const_iterator it;
    const std::string                     wireMessage = message.getRaw() + "\r\n";
    std::size_t                           recipientCount;

    recipientCount = 0;
    it = _connection.getClients().begin();
    while (it != _connection.getClients().end())
    {
        if ((*it)->getFd() != senderFd)
        {
            (*it)->appendSend(wireMessage);
            _connection.enableClientWrite((*it)->getFd());
            ++recipientCount;
        }
        ++it;
    }
    std::cout << "broadcast message from fd " << senderFd << " to "
        << recipientCount << " clients: " << message.getRaw() << std::endl;
}

void    ServerMessageHandler::sendToClient(int clientFd)
{
    Client  *client;
    ssize_t sent;

    client = _connection.findClient(clientFd);
    if (client == NULL)
        return ;
    while (client->hasPendingSend())
    {
        sent = send(clientFd, client->getSendData(), client->getSendSize(),
                MSG_NOSIGNAL);
        if (sent > 0)
        {
            client->removeSent(static_cast<std::string::size_type>(sent));
            std::cout << "sent " << sent << " bytes to fd " << clientFd
                << std::endl;
            continue ;
        }
        if (sent == -1 && errno == EINTR)
            continue ;
        if (sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return ;
        std::cerr << "send: " << std::strerror(errno) << std::endl;
        _connection.disconnectClient(clientFd);
        return ;
    }
    _connection.disableClientWrite(clientFd);
}
