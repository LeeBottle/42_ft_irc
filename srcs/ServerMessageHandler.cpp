#include "ServerMessageHandler.hpp"
#include "ServerConnection.hpp"

#include <cerrno>
#include <cctype>
#include <cstring>
#include <iostream>
#include <sys/socket.h>

ServerMessageHandler::ServerMessageHandler()
{
}

void    ServerMessageHandler::receiveClient(ServerConnection &connection,
    int clientFd)
{
    char    buffer[512];
    ssize_t received;
    Client  *client;

    client = connection.findClient(clientFd);
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
            printReceivedData(clientFd, buffer,
                static_cast<std::string::size_type>(received));
            processReceivedLines(connection, *client);
            continue ;
        }
        if (received == 0)
        {
            std::cout << "client disconnected with fd " << clientFd << std::endl;
            connection.disconnectClient(clientFd);
        }
        else if (errno == EINTR)
            continue ;
        else if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            std::cerr << "recv: " << std::strerror(errno) << std::endl;
            connection.disconnectClient(clientFd);
        }
        return ;
    }
}

void    ServerMessageHandler::printReceivedData(int clientFd, const char *data,
    std::string::size_type length) const
{
    std::string::size_type index;

    std::cout << "received data from fd " << clientFd << ": ";
    index = 0;
    while (index < length)
    {
        if (data[index] == '\r')
            std::cout << "\\r";
        else if (data[index] == '\n')
            std::cout << "\\n";
        else
            std::cout << data[index];
        ++index;
    }
    std::cout << std::endl;
}

void    ServerMessageHandler::processReceivedLines(ServerConnection &connection,
    Client &client)
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
        handleMessage(connection, client, message);
    }
}

void    ServerMessageHandler::handleMessage(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    const std::string command = toUpperCommand(message.getCommand());

    if (command == "PASS")
        handlePass(connection, client, message);
    else if (command == "NICK")
        handleNick(connection, client, message);
    else if (command == "USER")
        handleUser(connection, client, message);
    else if (!client.isRegistered())
        sendReply(connection, client, ":ircserv 451 * :You have not registered");
    else
        broadcastMessage(connection, client.getFd(), message);
}

void    ServerMessageHandler::handlePass(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    if (client.isRegistered())
    {
        sendReply(connection, client, ":ircserv 462 "
            + client.getNickname() + " :You may not reregister");
        return ;
    }
    if (message.getParameters().empty())
    {
        sendReply(connection, client,
            ":ircserv 461 * PASS :Not enough parameters");
        return ;
    }
    if (message.getParameters()[0] != connection.getPassword())
    {
        sendReply(connection, client, ":ircserv 464 * :Password incorrect");
        return ;
    }
    client.setPasswordAccepted();
    std::cout << "password accepted from fd " << client.getFd() << std::endl;
    tryRegister(connection, client);
}

void    ServerMessageHandler::handleNick(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    const std::string *nickname;

    if (message.getParameters().empty())
    {
        sendReply(connection, client, ":ircserv 431 * :No nickname given");
        return ;
    }
    nickname = &message.getParameters()[0];
    if (!isValidNickname(*nickname))
    {
        sendReply(connection, client, ":ircserv 432 * " + *nickname
            + " :Erroneous nickname");
        return ;
    }
    if (isNicknameInUse(connection, client, *nickname))
    {
        sendReply(connection, client, ":ircserv 433 * " + *nickname
            + " :Nickname is already in use");
        return ;
    }
    client.setNickname(*nickname);
    std::cout << "nickname set for fd " << client.getFd()
        << ": " << *nickname << std::endl;
    tryRegister(connection, client);
}

void    ServerMessageHandler::handleUser(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    if (client.isRegistered())
    {
        sendReply(connection, client, ":ircserv 462 "
            + client.getNickname() + " :You may not reregister");
        return ;
    }
    if (message.getParameters().size() < 4)
    {
        sendReply(connection, client,
            ":ircserv 461 * USER :Not enough parameters");
        return ;
    }
    client.setUser(message.getParameters()[0], message.getParameters()[3]);
    std::cout << "user set for fd " << client.getFd()
        << ": " << client.getUsername() << std::endl;
    tryRegister(connection, client);
}

void    ServerMessageHandler::tryRegister(ServerConnection &connection,
    Client &client)
{
    if (client.isRegistered())
        return ;
    if (!client.hasPassword() || !client.hasNickname()
        || !client.hasUsername())
        return ;
    client.setRegistered();
    sendReply(connection, client, ":ircserv 001 " + client.getNickname()
        + " :Welcome to the IRC server");
    std::cout << "client registered with fd " << client.getFd()
        << ": " << client.getNickname() << std::endl;
}

void    ServerMessageHandler::sendReply(ServerConnection &connection,
    Client &client,
    const std::string &message)
{
    client.appendSend(message + "\r\n");
    connection.enableClientWrite(client.getFd());
}

bool    ServerMessageHandler::isNicknameInUse(ServerConnection &connection,
    const Client &client,
    const std::string &nickname) const
{
    std::vector<Client *>::const_iterator it;

    it = connection.getClients().begin();
    while (it != connection.getClients().end())
    {
        if ((*it)->getFd() != client.getFd()
            && (*it)->getNickname() == nickname)
            return (true);
        ++it;
    }
    return (false);
}

bool    ServerMessageHandler::isValidNickname(const std::string &nickname) const
{
    std::string::size_type index;

    if (nickname.empty())
        return (false);
    index = 0;
    while (index < nickname.size())
    {
        if (nickname[index] == ' ' || nickname[index] == '\r'
            || nickname[index] == '\n' || nickname[index] == '\0')
            return (false);
        ++index;
    }
    return (true);
}

std::string ServerMessageHandler::toUpperCommand(
    const std::string &command) const
{
    std::string result;
    std::string::size_type index;

    result = command;
    index = 0;
    while (index < result.size())
    {
        result[index] = static_cast<char>(std::toupper(result[index]));
        ++index;
    }
    return (result);
}

void    ServerMessageHandler::broadcastMessage(ServerConnection &connection,
    int senderFd,
    const Message &message)
{
    std::vector<Client *>::const_iterator it;
    const std::string                     wireMessage = message.getRaw() + "\r\n";
    std::size_t                           recipientCount;

    recipientCount = 0;
    it = connection.getClients().begin();
    while (it != connection.getClients().end())
    {
        if ((*it)->getFd() != senderFd)
        {
            (*it)->appendSend(wireMessage);
            connection.enableClientWrite((*it)->getFd());
            ++recipientCount;
        }
        ++it;
    }
    std::cout << "broadcast message from fd " << senderFd << " to "
        << recipientCount << " clients: " << message.getRaw() << std::endl;
}

void    ServerMessageHandler::sendToClient(ServerConnection &connection,
    int clientFd)
{
    Client  *client;
    ssize_t sent;

    client = connection.findClient(clientFd);
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
        connection.disconnectClient(clientFd);
        return ;
    }
    connection.disableClientWrite(clientFd);
}
