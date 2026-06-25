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
            processReceivedLines(connection, *client);
            if (connection.findClient(clientFd) == NULL)
                return ;
            continue ;
        }
        if (received == 0)
        {
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

void    ServerMessageHandler::processReceivedLines(ServerConnection &connection,
    Client &client)
{
    std::string line;
    Message     message;

    while (client.extractLine(line))
    {
        const int clientFd = client.getFd();

        if (!message.parse(line))
        {
            continue ;
        }
        handleMessage(connection, client, message);
        if (connection.findClient(clientFd) == NULL)
            return ;
    }
}

void    ServerMessageHandler::handleMessage(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    const std::string command = toUpperCommand(message.getCommand());

    if (command == "CAP")
        handleCap(connection, client, message);
    else if (command == "PING")
        handlePing(connection, client, message);
    else if (command == "PONG")
        return ;
    else if (command == "QUIT")
        handleQuit(connection, client, message);
    else if (command == "PASS")
        handlePass(connection, client, message);
    else if (command == "NICK")
        handleNick(connection, client, message);
    else if (command == "USER")
        handleUser(connection, client, message);
    else if (!client.isRegistered())
        sendReply(connection, client, ":ircserv 451 * :You have not registered");
    else if (command == "PRIVMSG")
        handlePrivmsg(connection, client, message);
    else
        sendReply(connection, client, ":ircserv 421 "
            + getReplyTarget(client) + " " + command + " :Unknown command");
}

void    ServerMessageHandler::handlePass(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    if (client.isRegistered())
    {
        sendReply(connection, client, ":ircserv 462 "
            + client.getNickname()
            + " :Unauthorized command (already registered)");
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
    tryRegister(connection, client);
}

void    ServerMessageHandler::handleUser(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    if (client.isRegistered())
    {
        sendReply(connection, client, ":ircserv 462 "
            + client.getNickname()
            + " :Unauthorized command (already registered)");
        return ;
    }
    if (message.getParameters().size() < 4)
    {
        sendReply(connection, client,
            ":ircserv 461 * USER :Not enough parameters");
        return ;
    }
    client.setUser(message.getParameters()[0], message.getParameters()[3]);
    tryRegister(connection, client);
}

void    ServerMessageHandler::handlePrivmsg(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    const std::vector<std::string> &params = message.getParameters();
    Client                         *targetClient;
    std::string                    wireMessage;

    if (params.empty())
    {
        sendReply(connection, client, ":ircserv 411 "
            + getReplyTarget(client) + " :No recipient given (PRIVMSG)");
        return ;
    }
    if (params.size() < 2 || params[1].empty())
    {
        sendReply(connection, client, ":ircserv 412 "
            + getReplyTarget(client) + " :No text to send");
        return ;
    }
    targetClient = findClientByNickname(connection, params[0]);
    if (targetClient == NULL)
    {
        sendReply(connection, client, ":ircserv 401 "
            + getReplyTarget(client) + " " + params[0]
            + " :No such nick/channel");
        return ;
    }
    wireMessage = ":" + makeClientPrefix(client) + " PRIVMSG "
        + params[0] + " :" + params[1] + "\r\n";
    targetClient->appendSend(wireMessage);
    connection.enableClientWrite(targetClient->getFd());
}

void    ServerMessageHandler::handleCap(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    const std::vector<std::string> &params = message.getParameters();
    std::string                    subcommand;
    std::string                    target;

    if (params.empty())
        return ;
    subcommand = toUpperCommand(params[0]);
    target = getReplyTarget(client);
    if (subcommand == "LS")
        sendReply(connection, client, ":ircserv CAP " + target + " LS :");
    else if (subcommand == "LIST")
        sendReply(connection, client, ":ircserv CAP " + target + " LIST :");
    else if (subcommand == "REQ")
        sendReply(connection, client, ":ircserv CAP " + target + " NAK :");
}

void    ServerMessageHandler::handlePing(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    if (message.getParameters().empty())
    {
        sendReply(connection, client, ":ircserv 409 "
            + getReplyTarget(client) + " :No origin specified");
        return ;
    }
    sendReply(connection, client, "PONG :" + message.getParameters()[0]);
}

void    ServerMessageHandler::handleQuit(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    (void)message;
    connection.disconnectClient(client.getFd());
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
        + " :Welcome to the Internet Relay Network "
        + makeClientPrefix(client));
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

Client  *ServerMessageHandler::findClientByNickname(
    ServerConnection &connection,
    const std::string &nickname) const
{
    std::vector<Client *>::const_iterator it;

    it = connection.getClients().begin();
    while (it != connection.getClients().end())
    {
        if ((*it)->getNickname() == nickname)
            return (*it);
        ++it;
    }
    return (NULL);
}

std::string ServerMessageHandler::getReplyTarget(const Client &client) const
{
    if (client.hasNickname())
        return (client.getNickname());
    return ("*");
}

std::string ServerMessageHandler::makeClientPrefix(const Client &client) const
{
    return (client.getNickname() + "!" + client.getUsername()
        + "@localhost");
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
