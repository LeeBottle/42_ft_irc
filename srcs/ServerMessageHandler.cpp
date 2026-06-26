#include "ServerMessageHandler.hpp"
#include "ServerConnection.hpp"

#include <cerrno>
#include <cctype>
#include <cstring>
#include <iostream>
#include <sys/socket.h>

ServerMessageHandler::ServerMessageHandler()
    : _channels()
{
}

ServerMessageHandler::~ServerMessageHandler()
{
    std::vector<Channel *>::iterator it;

    it = _channels.begin();
    while (it != _channels.end())
    {
        delete *it;
        ++it;
    }
    _channels.clear();
}

void    ServerMessageHandler::receiveClient(ServerConnection &connection, int clientFd)
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
            client->appendReceived(buffer, static_cast<std::string::size_type>(received));
            processReceivedLines(connection, *client);
            if (connection.findClient(clientFd) == NULL)
                return ;
            continue ;
        }
        if (received == 0)
            connection.disconnectClient(clientFd);
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

void    ServerMessageHandler::processReceivedLines(ServerConnection &connection, Client &client)
{
    std::string line;
    Message     message;

    while (client.extractLine(line))
    {
        const int clientFd = client.getFd();

        if (!message.parse(line))
            continue ;
        handleMessage(connection, client, message);
        if (connection.findClient(clientFd) == NULL)
            return ;
    }
}

void    ServerMessageHandler::handleMessage(ServerConnection &connection, Client &client, const Message &message)
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
    else if (command == "JOIN")
        handleJoin(connection, client, message);
    else if (command == "PART")
        handlePart(connection, client, message);
    else if (command == "TOPIC")
        handleTopic(connection, client, message);
    else if (command == "MODE")
        handleMode(connection, client, message);
    else if (command == "WHO")
        handleWho(connection, client, message);
    else
        sendReply(connection, client, ":ircserv 421 " + getReplyTarget(client) + " " + command + " :Unknown command");
}

void    ServerMessageHandler::handlePass(ServerConnection &connection, Client &client, const Message &message)
{
    if (client.isRegistered())
    {
        sendReply(connection, client, ":ircserv 462 " + client.getNickname() + " :Unauthorized command (already registered)");
        return ;
    }
    if (message.getParameters().empty())
    {
        sendReply(connection, client, ":ircserv 461 * PASS :Not enough parameters");
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

void    ServerMessageHandler::handleNick(ServerConnection &connection, Client &client, const Message &message)
{
    const std::string*  nickname;

    if (message.getParameters().empty())
    {
        sendReply(connection, client, ":ircserv 431 * :No nickname given");
        return ;
    }
    nickname = &message.getParameters()[0];
    if (!isValidNickname(*nickname))
    {
        sendReply(connection, client, ":ircserv 432 * " + *nickname + " :Erroneous nickname");
        return ;
    }
    if (isNicknameInUse(connection, client, *nickname))
    {
        sendReply(connection, client, ":ircserv 433 * " + *nickname + " :Nickname is already in use");
        return ;
    }
    client.setNickname(*nickname);
    tryRegister(connection, client);
}

void    ServerMessageHandler::handleUser(ServerConnection &connection, Client &client, const Message &message)
{
    if (client.isRegistered())
    {
        sendReply(connection, client, ":ircserv 462 " + client.getNickname() + " :Unauthorized command (already registered)");
        return ;
    }
    if (message.getParameters().size() < 4)
    {
        sendReply(connection, client, ":ircserv 461 * USER :Not enough parameters");
        return ;
    }
    client.setUser(message.getParameters()[0], message.getParameters()[3]);
    tryRegister(connection, client);
}

void    ServerMessageHandler::handlePrivmsg(ServerConnection &connection, Client &client, const Message &message)
{
    const std::vector<std::string> &params = message.getParameters();
    Client                         *targetClient;
    Channel                        *targetChannel;
    std::string                    wireMessage;

    if (params.empty())
    {
        sendReply(connection, client, ":ircserv 411 " + getReplyTarget(client) + " :No recipient given (PRIVMSG)");
        return ;
    }
    if (params.size() < 2 || params[1].empty())
    {
        sendReply(connection, client, ":ircserv 412 " + getReplyTarget(client) + " :No text to send");
        return ;
    }
    if (isChannelName(params[0]))
    {
        targetChannel = findChannel(params[0]);
        if (targetChannel == NULL || !targetChannel->hasMember(&client))
        {
            sendReply(connection, client, ":ircserv 404 " + getReplyTarget(client) + " " + params[0] + " :Cannot send to channel");
            return ;
        }
        wireMessage = ":" + makeClientPrefix(client) + " PRIVMSG " + params[0] + " :" + params[1];
        sendToChannel(connection, targetChannel, wireMessage, &client);
        return ;
    }
    targetClient = findClientByNickname(connection, params[0]);
    if (targetClient == NULL)
    {
        sendReply(connection, client, ":ircserv 401 " + getReplyTarget(client) + " " + params[0] + " :No such nick/channel");
        return ;
    }
    wireMessage = ":" + makeClientPrefix(client) + " PRIVMSG " + params[0] + " :" + params[1] + "\r\n";
    targetClient->appendSend(wireMessage);
    connection.enableClientWrite(targetClient->getFd());
}

void    ServerMessageHandler::handleCap(ServerConnection &connection, Client &client, const Message &message)
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

void    ServerMessageHandler::handlePing(ServerConnection &connection, Client &client, const Message &message)
{
    if (message.getParameters().empty())
    {
        sendReply(connection, client, ":ircserv 409 " + getReplyTarget(client) + " :No origin specified");
        return ;
    }
    sendReply(connection, client, "PONG :" + message.getParameters()[0]);
}

void    ServerMessageHandler::handleQuit(ServerConnection &connection, Client &client, const Message &message)
{
    (void)message;
    connection.disconnectClient(client.getFd());
}

void    ServerMessageHandler::tryRegister(ServerConnection &connection, Client &client)
{
    if (client.isRegistered())
        return ;
    if (!client.hasPassword() || !client.hasNickname() || !client.hasUsername())
        return ;
    client.setRegistered();
    sendReply(connection, client, ":ircserv 001 " + client.getNickname() + " :Welcome to the Internet Relay Network " + makeClientPrefix(client));
}

void    ServerMessageHandler::sendReply(ServerConnection &connection, Client &client, const std::string &message)
{
    std::cout << "[SEND fd=" << client.getFd() << "] " << message << "\\r\\n" << std::endl;
    client.appendSend(message + "\r\n");
    connection.enableClientWrite(client.getFd());
}

bool    ServerMessageHandler::isNicknameInUse(ServerConnection &connection, const Client &client, const std::string &nickname) const
{
    std::vector<Client *>::const_iterator it;

    it = connection.getClients().begin();
    while (it != connection.getClients().end())
    {
        if ((*it)->getFd() != client.getFd() && (*it)->getNickname() == nickname)
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
        if (nickname[index] == ' ' || nickname[index] == '\r' || nickname[index] == '\n' || nickname[index] == '\0')
            return (false);
        ++index;
    }
    return (true);
}

std::string ServerMessageHandler::toUpperCommand(const std::string &command) const
{
    std::string             result;
    std::string::size_type  index;

    result = command;
    index = 0;
    while (index < result.size())
    {
        result[index] = static_cast<char>(std::toupper(result[index]));
        ++index;
    }
    return (result);
}

Client  *ServerMessageHandler::findClientByNickname(ServerConnection &connection, const std::string &nickname) const
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
    return (client.getNickname() + "!" + client.getUsername() + "@localhost");
}


Channel *ServerMessageHandler::findChannel(const std::string &name) const
{
    std::vector<Channel *>::const_iterator it;

    it = _channels.begin();
    while (it != _channels.end())
    {
        if ((*it)->getName() == name)
            return (*it);
        ++it;
    }
    return (NULL);
}

Channel *ServerMessageHandler::getOrCreateChannel(const std::string &name)
{
    Channel *channel;

    channel = findChannel(name);
    if (channel != NULL)
        return (channel);
    channel = new Channel(name);
    _channels.push_back(channel);
    return (channel);
}

bool    ServerMessageHandler::isChannelName(const std::string &name) const
{
    return (!name.empty() && name[0] == '#');
}

void    ServerMessageHandler::sendToChannel(ServerConnection &connection, Channel *channel, const std::string &message, Client *except)
{
    const std::vector<Client *>             &members = channel->getMembers();
    std::vector<Client *>::const_iterator   it;

    it = members.begin();
    while (it != members.end())
    {
        if (*it != except)
            sendReply(connection, **it, message);
        ++it;
    }
}

std::string ServerMessageHandler::makeNamesList(Channel *channel) const
{
    const std::vector<Client *>             &members = channel->getMembers();
    std::vector<Client *>::const_iterator   it;
    std::string                              names;

    it = members.begin();
    while (it != members.end())
    {
        if (!names.empty())
            names += " ";
        names += (*it)->getNickname();
        ++it;
    }
    return (names);
}

void    ServerMessageHandler::sendNamesReply(ServerConnection &connection, Client &client, Channel *channel)
{
    std::string target;
    std::string channelName;
    std::string names;

    target = getReplyTarget(client);
    channelName = channel->getName();
    names = makeNamesList(channel);
    sendReply(connection, client, ":ircserv 353 " + target + " = " + channelName + " :" + names);
    sendReply(connection, client, ":ircserv 366 " + target + " " + channelName + " :End of /NAMES list.");
}

void    ServerMessageHandler::sendTopicReply(ServerConnection &connection, Client &client, Channel *channel)
{
    std::string target;
    std::string channelName;

    target = getReplyTarget(client);
    channelName = channel->getName();
    if (channel->hasTopic())
        sendReply(connection, client, ":ircserv 332 " + target + " " + channelName + " :" + channel->getTopic());
    else
        sendReply(connection, client, ":ircserv 331 " + target + " " + channelName + " :No topic is set");
}

void    ServerMessageHandler::handleJoin(ServerConnection &connection, Client &client, const Message &message)
{
    const std::vector<std::string>    &params = message.getParameters();
    std::string                       channelName;
    Channel                           *channel;
    std::string                       joinMessage;

    if (params.empty())
    {
        sendReply(connection, client, ":ircserv 461 " + getReplyTarget(client) + " JOIN :Not enough parameters");
        return ;
    }
    channelName = params[0];
    if (!isChannelName(channelName))
    {
        sendReply(connection, client, ":ircserv 403 " + getReplyTarget(client) + " " + channelName + " :No such channel");
        return ;
    }
    channel = getOrCreateChannel(channelName);
    if (!channel->hasMember(&client))
    {
        channel->addMember(&client);
        joinMessage = ":" + makeClientPrefix(client) + " JOIN :" + channelName;
        sendToChannel(connection, channel, joinMessage, NULL);
    }
    if (channel->hasTopic())
        sendTopicReply(connection, client, channel);
    sendNamesReply(connection, client, channel);
}

void    ServerMessageHandler::handlePart(ServerConnection &connection, Client &client, const Message &message)
{
    const std::vector<std::string>    &params = message.getParameters();
    std::string                       channelName;
    Channel                           *channel;
    std::string                       partMessage;

    if (params.empty())
    {
        sendReply(connection, client, ":ircserv 461 " + getReplyTarget(client) + " PART :Not enough parameters");
        return ;
    }
    channelName = params[0];
    channel = findChannel(channelName);
    if (channel == NULL)
    {
        sendReply(connection, client, ":ircserv 403 " + getReplyTarget(client) + " " + channelName + " :No such channel");
        return ;
    }
    if (!channel->hasMember(&client))
    {
        sendReply(connection, client, ":ircserv 442 " + getReplyTarget(client) + " " + channelName + " :You're not on that channel");
        return ;
    }
    partMessage = ":" + makeClientPrefix(client) + " PART " + channelName + " :Leaving";
    sendToChannel(connection, channel, partMessage, NULL);
    channel->removeMember(&client);
    deleteChannelIfEmpty(channel);
}

void    ServerMessageHandler::handleTopic(ServerConnection &connection, Client& client, const Message& message)
{
    const std::vector<std::string>  &params = message.getParameters();
    std::string                     channelName;
    Channel*                        channel;
    std::string                     topicMessage;
    std::string                     topic;

    if (params.empty())
    {
        sendReply(connection, client, ":ircserv 461 " + getReplyTarget(client) + " TOPIC :Not enough parameters");
        return ;
    }
    channelName = params[0];
    channel = findChannel(channelName);
    if (channel == NULL)
    {
        sendReply(connection, client, ":ircserv 403 " + getReplyTarget(client) + " " + channelName + " :No such channel");
        return ;
    }
    if (params.size() == 1)
    {
        sendTopicReply(connection, client, channel);
        return ;
    }
    if (!channel->hasMember(&client))
    {
        sendReply(connection, client, ":ircserv 442 " + getReplyTarget(client) + " " + channelName + " :You're not on that channel");
        return ;
    }

    topic = params[1];
    if (topic == ":")
        topic.clear();
    channel->setTopic(topic);
    topicMessage = ":" + makeClientPrefix(client) + " TOPIC " + channelName + " :" + topic;
    sendToChannel(connection, channel, topicMessage, NULL);
}

void    ServerMessageHandler::handleMode(ServerConnection &connection, Client &client, const Message &message)
{
    (void)message;
    sendReply(connection, client, ":ircserv 221 " + getReplyTarget(client) + " +i");
}

void    ServerMessageHandler::handleWho(ServerConnection &connection, Client &client, const Message &message)
{
    const std::vector<std::string>&         params = message.getParameters();
    const std::vector<Client *>*            members;
    std::vector<Client *>::const_iterator   it;
    std::string                             target;
    std::string                             channelName;
    std::string                             realname;
    Channel*                                channel;

    target = getReplyTarget(client);
    if (params.empty())
    {
        sendReply(connection, client, ":ircserv 461 " + target + " WHO :Not enough parameters");
        return ;
    }
    channelName = params[0];
    channel = findChannel(channelName);
    if (channel != NULL)
    {
        members = &channel->getMembers();
        it = members->begin();
        while (it != members->end())
        {
            realname = (*it)->getRealname();
            if (realname.empty())
                realname = (*it)->getNickname();
            sendReply(connection, client, ":ircserv 352 " + target + " " + channelName + " " + (*it)->getUsername()
                + " localhost ircserv " + (*it)->getNickname() + " H :0 " + realname);
            ++it;
        }
    }
    sendReply(connection, client, ":irecserv 315 " + target + " " + channelName + " :End of WHO list");
}

void    ServerMessageHandler::removeClientFromChannels(Client &client)
{
    std::vector<Channel *>::iterator it;

    it = _channels.begin();
    while (it != _channels.end())
    {
        (*it)->removeMember(&client);
        if ((*it)->isEmpty())
        {
            delete *it;
            it = _channels.erase(it);
        }
        else
            ++it;
    }
}

void    ServerMessageHandler::sendToClient(ServerConnection &connection, int clientFd)
{
    Client  *client;
    ssize_t sent;

    client = connection.findClient(clientFd);
    if (client == NULL)
        return ;
    while (client->hasPendingSend())
    {
        sent = send(clientFd, client->getSendData(), client->getSendSize(), MSG_NOSIGNAL);
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

void    ServerMessageHandler::deleteChannelIfEmpty(Channel *channel)
{
    std::vector<Channel *>::iterator it;

    if (channel == NULL || !channel->isEmpty())
        return ;
    it = _channels.begin();
    while (it != _channels.end())
    {
        if (*it == channel)
        {
            delete *it;
            _channels.erase(it);
            return ;
        }
        ++it;
    }
}
