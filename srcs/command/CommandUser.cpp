#include "command/CommandUser.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "parser/Parser.hpp"

#include <sys/socket.h>
#include <vector>

CommandUser::CommandUser(const std::string &password, 
    ClientManager &clients)
    : _password(password), _clients(clients)
{
}

CommandUser::~CommandUser()
{
}

void    CommandUser::executePass(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.getParams();
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

void    CommandUser::executeNick(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.getParams();
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

void    CommandUser::executeUser(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.getParams();
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

void    CommandUser::executeCap(Client &client, const Parser &message)
{
    (void)message;
    queueReply(client, ":ircserv CAP * LS :\r\n");
}

void    CommandUser::executePing(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.getParams();

    if (params.empty())
    {
        queueReply(client, ":ircserv 409 " + getReplyTarget(client)
            + " :No origin specified\r\n");
        return ;
    }
    queueReply(client, ":ircserv PONG ircserv :" + params[0] + "\r\n");
}

void    CommandUser::executePong(Client &client, const Parser &message)
{
    (void)client;
    (void)message;
}

void    CommandUser::executeQuit(Client &client, const Parser &message)
{
    (void)message;
    send(client.getFd(), ":ircserv ERROR :Closing Link\r\n",
        sizeof(":ircserv ERROR :Closing Link\r\n") - 1, 0);
}

void    CommandUser::executeUnknown(Client &client, const Parser &message)
{
    queueReply(client, ":ircserv 421 " + getReplyTarget(client)
        + " " + message.getName() + " :Unknown command\r\n");
}

void    CommandUser::sendRegistrationIfReady(Client &client,
        bool wasRegistered)
{
    if (wasRegistered || !client.isRegistered())
        return ;
    queueReply(client, ":ircserv 001 " + client.getNickname()
        + " :Welcome to ircserv\r\n");
    queueReply(client, ":ircserv 221 " + client.getNickname() + " +i\r\n");
}

void    CommandUser::queueReply(Client &client, const std::string &message)
{
    client.queueSend(message);
}

const std::string   &CommandUser::getReplyTarget(Client &client) const
{
    static const std::string unknownTarget = "*";

    if (client.hasNickname())
        return (client.getNickname());
    return (unknownTarget);
}
