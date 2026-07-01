#include "command/CommandUserHandler.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "command/Command.hpp"

#include <signal.h>
#include <sys/socket.h>
#include <vector>

CommandUserHandler::CommandUserHandler(const std::string &password, 
    ClientManager &clients)
    : _password(password), _clients(clients)
{
}

CommandUserHandler::~CommandUserHandler()
{
}

void    CommandUserHandler::handlePass(Client &client, const Command &command)
{
    const std::vector<std::string>  &params = command.getParams();
    bool                            wasRegistered;

    wasRegistered = client.isRegistered();
    if (client.isRegistered())
    {
        queueReply(client, ":ircserv 462 " + getReplyTarget(client)
            + " :You may not reregister\r\n");
        return ;
    }
    if (params.empty())
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " PASS :Not enough parameters\r\n");
        return ;
    }
    if (params[0] != _password)
    {
        queueReply(client, ":ircserv 464 " + getReplyTarget(client)
            + " :Password incorrect\r\n");
        return ;
    }
    client.acceptPassword();
    sendRegistrationIfReady(client, wasRegistered);
}

void    CommandUserHandler::handleNick(Client &client, const Command &command)
{
    const std::vector<std::string>  &params = command.getParams();
    bool                            wasRegistered;

    wasRegistered = client.isRegistered();
    if (params.empty())
    {
        queueReply(client, ":ircserv 431 " + getReplyTarget(client)
            + " :No nickname given\r\n");
        return ;
    }
    if (_clients.isNicknameInUse(params[0], client))
    {
        queueReply(client, ":ircserv 433 " + getReplyTarget(client)
            + " " + params[0] + " :Nickname is already in use\r\n");
        return ;
    }
    client.setNickname(params[0]);
    sendRegistrationIfReady(client, wasRegistered);
}

void    CommandUserHandler::handleUser(Client &client, const Command &command)
{
    const std::vector<std::string>  &params = command.getParams();
    bool                            wasRegistered;

    wasRegistered = client.isRegistered();
    if (client.isRegistered())
    {
        queueReply(client, ":ircserv 462 " + getReplyTarget(client)
            + " :You may not reregister\r\n");
        return ;
    }
    if (params.size() < 4)
    {
        queueReply(client, ":ircserv 461 " + getReplyTarget(client)
            + " USER :Not enough parameters\r\n");
        return ;
    }
    client.setUser(params[0], params[3]);
    sendRegistrationIfReady(client, wasRegistered);
}

void    CommandUserHandler::handleCap(Client &client, const Command &command)
{
    (void)command;
    queueReply(client, ":ircserv CAP * LS :\r\n");
}

void    CommandUserHandler::handlePing(Client &client, const Command &command)
{
    const std::vector<std::string>  &params = command.getParams();

    if (params.empty())
    {
        queueReply(client, ":ircserv 409 " + getReplyTarget(client)
            + " :No origin specified\r\n");
        return ;
    }
    queueReply(client, ":ircserv PONG ircserv :" + params[0] + "\r\n");
}

void    CommandUserHandler::handlePong(Client &client, const Command &command)
{
    (void)client;
    (void)command;
}

void    CommandUserHandler::handleDie(Client &client, const Command &command)
{
    (void)command;
    if (!client.isRegistered())
    {
        queueReply(client, ":ircserv 451 " + getReplyTarget(client)
            + " :You have not registered\r\n");
        return ;
    }
    send(client.getFd(), ":ircserv ERROR :Server shutting down\r\n",
        sizeof(":ircserv ERROR :Server shutting down\r\n") - 1, 0);
    raise(SIGTERM);
}

void    CommandUserHandler::handleUnknown(Client &client, const Command &command)
{
    queueReply(client, ":ircserv 421 " + getReplyTarget(client)
        + " " + command.getName() + " :Unknown command\r\n");
}

void    CommandUserHandler::sendRegistrationIfReady(Client &client,
        bool wasRegistered)
{
    if (wasRegistered || !client.isRegistered())
        return ;
    queueReply(client, ":ircserv 001 " + client.getNickname()
        + " :Welcome to ircserv\r\n");
    queueReply(client, ":ircserv 221 " + client.getNickname() + " +i\r\n");
}

void    CommandUserHandler::queueReply(Client &client, const std::string &message)
{
    client.queueSend(message);
}

const std::string   &CommandUserHandler::getReplyTarget(Client &client) const
{
    static const std::string unknownTarget = "*";

    if (client.hasNickname())
        return (client.getNickname());
    return (unknownTarget);
}
