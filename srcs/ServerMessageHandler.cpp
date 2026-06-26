#include "ServerMessageHandler.hpp"
#include "ServerConnection.hpp"

#include <cerrno>
#include <cctype>
#include <cstring>
#include <iostream>
#include <sstream>
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
    else if (command == "JOIN")
        handleJoin(connection, client, message);
    else if (command == "PART")
        handlePart(connection, client, message);
    else if (command == "TOPIC")
        handleTopic(connection, client, message);
    else if (command == "INVITE")
        handleInvite(connection, client, message);
    else if (command == "KICK")
        handleKick(connection, client, message);
    else if (command == "MODE")
        handleMode(connection, client, message);
    else if (command == "WHO")
        handleWho(connection, client, message);
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
    Channel                        *targetChannel;
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
    if (isChannelName(params[0]))
    {
        targetChannel = findChannel(params[0]);
        if (targetChannel == NULL || !targetChannel->hasMember(&client))
        {
            sendReply(connection, client, ":ircserv 404 "
                + getReplyTarget(client) + " " + params[0]
                + " :Cannot send to channel");
            return ;
        }
        wireMessage = ":" + makeClientPrefix(client) + " PRIVMSG "
            + params[0] + " :" + params[1];
        sendToChannel(connection, targetChannel, wireMessage, &client);
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
    std::cout << "[SEND fd=" << client.getFd() << "] "
        << message << "\\r\\n" << std::endl;
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

bool    ServerMessageHandler::parseLimit(const std::string &text,
    unsigned int &limit) const
{
    std::string::size_type index;
    unsigned int           value;

    if (text.empty())
        return (false);
    index = 0;
    value = 0;
    while (index < text.size())
    {
        if (text[index] < '0' || text[index] > '9')
            return (false);
        value = value * 10 + static_cast<unsigned int>(text[index] - '0');
        if (value == 0 || value > 100000)
            return (false);
        ++index;
    }
    limit = value;
    return (true);
}

void    ServerMessageHandler::addModeChange(std::string &modeChanges,
    char &currentSign,
    char sign,
    char mode) const
{
    if (currentSign != sign)
    {
        modeChanges += sign;
        currentSign = sign;
    }
    modeChanges += mode;
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

void    ServerMessageHandler::sendToChannel(ServerConnection &connection,
    Channel *channel,
    const std::string &message,
    Client *except)
{
    const std::vector<Client *>       &members = channel->getMembers();
    std::vector<Client *>::const_iterator it;

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
    const std::vector<Client *>       &members = channel->getMembers();
    std::vector<Client *>::const_iterator it;
    std::string                       names;

    it = members.begin();
    while (it != members.end())
    {
        if (!names.empty())
            names += " ";
        if (channel->hasOperator(*it))
            names += "@";
        names += (*it)->getNickname();
        ++it;
    }
    return (names);
}

void    ServerMessageHandler::sendNamesReply(ServerConnection &connection,
    Client &client,
    Channel *channel)
{
    std::string target;
    std::string channelName;
    std::string names;

    target = getReplyTarget(client);
    channelName = channel->getName();
    names = makeNamesList(channel);
    sendReply(connection, client, ":ircserv 353 " + target
        + " = " + channelName + " :" + names);
    sendReply(connection, client, ":ircserv 366 " + target
        + " " + channelName + " :End of /NAMES list.");
}

void    ServerMessageHandler::sendTopicReply(ServerConnection &connection,
    Client &client,
    Channel *channel)
{
    std::string target;
    std::string channelName;

    target = getReplyTarget(client);
    channelName = channel->getName();
    if (channel->hasTopic())
        sendReply(connection, client, ":ircserv 332 " + target
            + " " + channelName + " :" + channel->getTopic());
    else
        sendReply(connection, client, ":ircserv 331 " + target
            + " " + channelName + " :No topic is set");
}

void    ServerMessageHandler::sendChannelModeReply(ServerConnection &connection,
    Client &client,
    Channel *channel)
{
    std::string modes;
    std::string params;
    std::ostringstream limitStream;

    modes = "+";
    if (channel->isInviteOnly())
        modes += "i";
    if (channel->isTopicRestricted())
        modes += "t";
    if (channel->hasKey())
    {
        modes += "k";
        params += " " + channel->getKey();
    }
    if (channel->hasLimit())
    {
        modes += "l";
        limitStream << channel->getLimit();
        params += " " + limitStream.str();
    }
    sendReply(connection, client, ":ircserv 324 " + getReplyTarget(client)
        + " " + channel->getName() + " " + modes + params);
}

void    ServerMessageHandler::handleJoin(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    const std::vector<std::string>    &params = message.getParameters();
    std::string                       channelName;
    Channel                           *channel;
    std::string                       joinMessage;
    bool                              firstMember;

    if (params.empty())
    {
        sendReply(connection, client, ":ircserv 461 "
            + getReplyTarget(client) + " JOIN :Not enough parameters");
        return ;
    }
    channelName = params[0];
    if (!isChannelName(channelName))
    {
        sendReply(connection, client, ":ircserv 403 "
            + getReplyTarget(client) + " " + channelName
            + " :No such channel");
        return ;
    }
    channel = getOrCreateChannel(channelName);
    firstMember = channel->isEmpty();
    if (!channel->hasMember(&client) && !firstMember && channel->isInviteOnly()
        && !channel->hasInvitation(&client))
    {
        sendReply(connection, client, ":ircserv 473 "
            + getReplyTarget(client) + " " + channelName
            + " :Cannot join channel (+i)");
        return ;
    }
    if (!channel->hasMember(&client) && !firstMember && channel->hasKey()
        && (params.size() < 2 || params[1] != channel->getKey()))
    {
        sendReply(connection, client, ":ircserv 475 "
            + getReplyTarget(client) + " " + channelName
            + " :Cannot join channel (+k)");
        return ;
    }
    if (!channel->hasMember(&client) && !firstMember && channel->hasLimit()
        && channel->getMembers().size() >= channel->getLimit())
    {
        sendReply(connection, client, ":ircserv 471 "
            + getReplyTarget(client) + " " + channelName
            + " :Cannot join channel (+l)");
        return ;
    }
    if (!channel->hasMember(&client))
    {
        channel->addMember(&client);
        if (firstMember)
            channel->addOperator(&client);
        channel->removeInvitation(&client);
        joinMessage = ":" + makeClientPrefix(client) + " JOIN :"
            + channelName;
        sendToChannel(connection, channel, joinMessage, NULL);
    }
    if (channel->hasTopic())
        sendTopicReply(connection, client, channel);
    sendNamesReply(connection, client, channel);
}

void    ServerMessageHandler::handlePart(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    const std::vector<std::string>    &params = message.getParameters();
    std::string                       channelName;
    Channel                           *channel;
    std::string                       partMessage;

    if (params.empty())
    {
        sendReply(connection, client, ":ircserv 461 "
            + getReplyTarget(client) + " PART :Not enough parameters");
        return ;
    }
    channelName = params[0];
    channel = findChannel(channelName);
    if (channel == NULL)
    {
        sendReply(connection, client, ":ircserv 403 "
            + getReplyTarget(client) + " " + channelName
            + " :No such channel");
        return ;
    }
    if (!channel->hasMember(&client))
    {
        sendReply(connection, client, ":ircserv 442 "
            + getReplyTarget(client) + " " + channelName
            + " :You're not on that channel");
        return ;
    }
    partMessage = ":" + makeClientPrefix(client) + " PART " + channelName + " :Leaving";
    sendToChannel(connection, channel, partMessage, NULL);
    channel->removeMember(&client);
    deleteChannelIfEmpty(channel);
}

void    ServerMessageHandler::handleTopic(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    const std::vector<std::string>    &params = message.getParameters();
    std::string                       channelName;
    Channel                           *channel;
    std::string                       topicMessage;

    if (params.empty())
    {
        sendReply(connection, client, ":ircserv 461 "
            + getReplyTarget(client) + " TOPIC :Not enough parameters");
        return ;
    }
    channelName = params[0];
    channel = findChannel(channelName);
    if (channel == NULL)
    {
        sendReply(connection, client, ":ircserv 403 "
            + getReplyTarget(client) + " " + channelName
            + " :No such channel");
        return ;
    }
    if (params.size() == 1)
    {
        sendTopicReply(connection, client, channel);
        return ;
    }
    if (!channel->hasMember(&client))
    {
        sendReply(connection, client, ":ircserv 442 "
            + getReplyTarget(client) + " " + channelName
            + " :You're not on that channel");
        return ;
    }
    if (channel->isTopicRestricted() && !channel->hasOperator(&client))
    {
        sendReply(connection, client, ":ircserv 482 "
            + getReplyTarget(client) + " " + channelName
            + " :You're not channel operator");
        return ;
    }
    channel->setTopic(params[1]);
    topicMessage = ":" + makeClientPrefix(client) + " TOPIC "
        + channelName + " :" + params[1];
    sendToChannel(connection, channel, topicMessage, NULL);
}

void    ServerMessageHandler::handleInvite(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    const std::vector<std::string>    &params = message.getParameters();
    Client                            *targetClient;
    Channel                           *channel;
    std::string                       inviteMessage;

    if (params.size() < 2)
    {
        sendReply(connection, client, ":ircserv 461 "
            + getReplyTarget(client) + " INVITE :Not enough parameters");
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
    channel = findChannel(params[1]);
    if (channel == NULL)
    {
        sendReply(connection, client, ":ircserv 403 "
            + getReplyTarget(client) + " " + params[1]
            + " :No such channel");
        return ;
    }
    if (!channel->hasMember(&client))
    {
        sendReply(connection, client, ":ircserv 442 "
            + getReplyTarget(client) + " " + params[1]
            + " :You're not on that channel");
        return ;
    }
    if (!channel->hasOperator(&client))
    {
        sendReply(connection, client, ":ircserv 482 "
            + getReplyTarget(client) + " " + params[1]
            + " :You're not channel operator");
        return ;
    }
    if (channel->hasMember(targetClient))
    {
        sendReply(connection, client, ":ircserv 443 "
            + getReplyTarget(client) + " " + targetClient->getNickname()
            + " " + params[1] + " :is already on channel");
        return ;
    }
    channel->addInvitation(targetClient);
    sendReply(connection, client, ":ircserv 341 " + getReplyTarget(client)
        + " " + targetClient->getNickname() + " " + params[1]);
    inviteMessage = ":" + makeClientPrefix(client) + " INVITE "
        + targetClient->getNickname() + " :" + params[1] + "\r\n";
    targetClient->appendSend(inviteMessage);
    connection.enableClientWrite(targetClient->getFd());
}

void    ServerMessageHandler::handleKick(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    const std::vector<std::string>    &params = message.getParameters();
    Channel                           *channel;
    Client                            *targetClient;
    std::string                       comment;
    std::string                       kickMessage;

    if (params.size() < 2)
    {
        sendReply(connection, client, ":ircserv 461 "
            + getReplyTarget(client) + " KICK :Not enough parameters");
        return ;
    }
    channel = findChannel(params[0]);
    if (channel == NULL)
    {
        sendReply(connection, client, ":ircserv 403 "
            + getReplyTarget(client) + " " + params[0]
            + " :No such channel");
        return ;
    }
    if (!channel->hasMember(&client))
    {
        sendReply(connection, client, ":ircserv 442 "
            + getReplyTarget(client) + " " + params[0]
            + " :You're not on that channel");
        return ;
    }
    if (!channel->hasOperator(&client))
    {
        sendReply(connection, client, ":ircserv 482 "
            + getReplyTarget(client) + " " + params[0]
            + " :You're not channel operator");
        return ;
    }
    targetClient = findClientByNickname(connection, params[1]);
    if (targetClient == NULL || !channel->hasMember(targetClient))
    {
        sendReply(connection, client, ":ircserv 441 "
            + getReplyTarget(client) + " " + params[1] + " " + params[0]
            + " :They aren't on that channel");
        return ;
    }
    comment = client.getNickname();
    if (params.size() > 2 && !params[2].empty())
        comment = params[2];
    kickMessage = ":" + makeClientPrefix(client) + " KICK "
        + params[0] + " " + targetClient->getNickname() + " :" + comment;
    sendToChannel(connection, channel, kickMessage, NULL);
    channel->removeMember(targetClient);
    deleteChannelIfEmpty(channel);
}

void    ServerMessageHandler::handleMode(ServerConnection &connection,
    Client &client,
    const Message &message)
{
    const std::vector<std::string>    &params = message.getParameters();
    Channel                           *channel;
    Client                            *targetClient;
    std::string                       modeChanges;
    std::string                       modeParams;
    std::string                       modeString;
    std::string::size_type            index;
    std::string::size_type            paramIndex;
    unsigned int                      limit;
    char                              sign;
    char                              currentSign;

    if (params.empty())
    {
        sendReply(connection, client, ":ircserv 461 "
            + getReplyTarget(client) + " MODE :Not enough parameters");
        return ;
    }
    if (!isChannelName(params[0]))
    {
        sendReply(connection, client, ":ircserv 221 "
            + getReplyTarget(client) + " +i");
        return ;
    }
    channel = findChannel(params[0]);
    if (channel == NULL)
    {
        sendReply(connection, client, ":ircserv 403 "
            + getReplyTarget(client) + " " + params[0]
            + " :No such channel");
        return ;
    }
    if (params.size() == 1)
    {
        sendChannelModeReply(connection, client, channel);
        return ;
    }
    if (!channel->hasMember(&client))
    {
        sendReply(connection, client, ":ircserv 442 "
            + getReplyTarget(client) + " " + params[0]
            + " :You're not on that channel");
        return ;
    }
    if (!channel->hasOperator(&client))
    {
        sendReply(connection, client, ":ircserv 482 "
            + getReplyTarget(client) + " " + params[0]
            + " :You're not channel operator");
        return ;
    }
    modeString = params[1];
    index = 0;
    paramIndex = 2;
    sign = '+';
    currentSign = '\0';
    while (index < modeString.size())
    {
        if (modeString[index] == '+' || modeString[index] == '-')
            sign = modeString[index];
        else if (modeString[index] == 'i')
        {
            if (sign == '+' && !channel->isInviteOnly())
            {
                channel->setInviteOnly(true);
                addModeChange(modeChanges, currentSign, sign, 'i');
            }
            else if (sign == '-' && channel->isInviteOnly())
            {
                channel->setInviteOnly(false);
                addModeChange(modeChanges, currentSign, sign, 'i');
            }
        }
        else if (modeString[index] == 't')
        {
            if (sign == '+' && !channel->isTopicRestricted())
            {
                channel->setTopicRestricted(true);
                addModeChange(modeChanges, currentSign, sign, 't');
            }
            else if (sign == '-' && channel->isTopicRestricted())
            {
                channel->setTopicRestricted(false);
                addModeChange(modeChanges, currentSign, sign, 't');
            }
        }
        else if (modeString[index] == 'k')
        {
            if (sign == '+')
            {
                if (paramIndex >= params.size() || params[paramIndex].empty())
                {
                    sendReply(connection, client, ":ircserv 461 "
                        + getReplyTarget(client)
                        + " MODE :Not enough parameters");
                    return ;
                }
                channel->setKey(params[paramIndex]);
                addModeChange(modeChanges, currentSign, sign, 'k');
                modeParams += " " + params[paramIndex];
                ++paramIndex;
            }
            else if (channel->hasKey())
            {
                channel->clearKey();
                addModeChange(modeChanges, currentSign, sign, 'k');
            }
        }
        else if (modeString[index] == 'l')
        {
            if (sign == '+')
            {
                if (paramIndex >= params.size()
                    || !parseLimit(params[paramIndex], limit))
                {
                    sendReply(connection, client, ":ircserv 461 "
                        + getReplyTarget(client)
                        + " MODE :Not enough parameters");
                    return ;
                }
                channel->setLimit(limit);
                addModeChange(modeChanges, currentSign, sign, 'l');
                modeParams += " " + params[paramIndex];
                ++paramIndex;
            }
            else if (channel->hasLimit())
            {
                channel->clearLimit();
                addModeChange(modeChanges, currentSign, sign, 'l');
            }
        }
        else if (modeString[index] == 'o')
        {
            if (paramIndex >= params.size())
            {
                sendReply(connection, client, ":ircserv 461 "
                    + getReplyTarget(client)
                    + " MODE :Not enough parameters");
                return ;
            }
            targetClient = findClientByNickname(connection, params[paramIndex]);
            if (targetClient == NULL || !channel->hasMember(targetClient))
            {
                sendReply(connection, client, ":ircserv 441 "
                    + getReplyTarget(client) + " " + params[paramIndex]
                    + " " + params[0]
                    + " :They aren't on that channel");
                return ;
            }
            if (sign == '+' && !channel->hasOperator(targetClient))
            {
                channel->addOperator(targetClient);
                addModeChange(modeChanges, currentSign, sign, 'o');
                modeParams += " " + targetClient->getNickname();
            }
            else if (sign == '-' && channel->hasOperator(targetClient))
            {
                channel->removeOperator(targetClient);
                addModeChange(modeChanges, currentSign, sign, 'o');
                modeParams += " " + targetClient->getNickname();
            }
            ++paramIndex;
        }
        else
        {
            sendReply(connection, client, ":ircserv 472 "
                + getReplyTarget(client) + " " + modeString[index]
                + " :is unknown mode char to me");
            return ;
        }
        ++index;
    }
    if (!modeChanges.empty())
        sendToChannel(connection, channel, ":" + makeClientPrefix(client)
            + " MODE " + params[0] + " " + modeChanges + modeParams, NULL);
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
    sendReply(connection, client, ":ircserv 315 " + target + " " + channelName + " :End of WHO list");
}

void    ServerMessageHandler::removeClientFromChannels(Client &client)
{
    std::vector<Channel *>::iterator it;

    it = _channels.begin();
    while (it != _channels.end())
    {
        (*it)->removeInvitation(&client);
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
