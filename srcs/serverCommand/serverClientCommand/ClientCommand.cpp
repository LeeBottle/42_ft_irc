#include "serverCommand/serverClientCommand/ClientCommand.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"
#include "parser/Parser.hpp"

#include <vector>


// Initializes this object with the supplied state.
ClientCommand::ClientCommand(const std::string &password,
    ClientManager &clients)
    : _password(password), _clients(clients)
{
}


// Destroys this object and releases its owned resources.
ClientCommand::~ClientCommand()
{
}


// Performs the pass operation.
bool    ClientCommand::pass(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.params();
    bool                            wasRegistered;

    wasRegistered = client.isRegistered();
    if (client.isRegistered())
    {
        reply(client, ":ircserv 462 " + target(client)
            + " :You may not reregister\r\n");

        return (true);
    }

    if (params.empty())
    {
        reply(client, ":ircserv 461 " + target(client)
            + " PASS :Not enough parameters\r\n");

        return (true);
    }

    if (params[0] != _password)
    {
        reply(client, ":ircserv 464 " + target(client)
            + " :Password incorrect\r\n");

        return (true);
    }

    client.acceptPassword();
    sendRegistrationIfReady(client, wasRegistered);

    return (true);
}


// Performs the nick operation.
bool    ClientCommand::nick(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.params();
    bool                            wasRegistered;

    wasRegistered = client.isRegistered();

    if (params.empty())
    {
        reply(client, ":ircserv 431 " + target(client)
            + " :No nickname given\r\n");

        return (true);
    }

    if (_clients.isNicknameInUse(params[0], client))
    {
        reply(client, ":ircserv 433 " + target(client)
            + " " + params[0] + " :Nickname is already in use\r\n");

        return (true);
    }

    client.setNickname(params[0]);
    sendRegistrationIfReady(client, wasRegistered);

    return (true);
}


// Performs the user operation.
bool    ClientCommand::user(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.params();
    bool                            wasRegistered;

    wasRegistered = client.isRegistered();

    if (client.isRegistered())
    {
        reply(client, ":ircserv 462 " + target(client)
            + " :You may not reregister\r\n");

        return (true);
    }

    if (params.size() < 4)
    {
        reply(client, ":ircserv 461 " + target(client)
            + " USER :Not enough parameters\r\n");

        return (true);
    }

    client.setUser(params[0], params[3]);
    sendRegistrationIfReady(client, wasRegistered);

    return (true);
}


// Performs the cap operation.
bool    ClientCommand::cap(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.params();

    if (!params.empty() && params[0] == "LS")
        reply(client, ":ircserv CAP * LS :\r\n");

    return (true);
}


// Performs the ping operation.
bool    ClientCommand::ping(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.params();

    if (params.empty())
    {
        reply(client, ":ircserv 409 " + target(client)
            + " :No origin specified\r\n");

        return (true);
    }

    reply(client, ":ircserv PONG ircserv :" + params[0] + "\r\n");

    return (true);
}


// Performs the pong operation.
bool    ClientCommand::pong(Client &client, const Parser &message)
{
    (void)client;
    (void)message;

    return (true);
}


// Performs the quit operation.
bool    ClientCommand::quit(Client &client, const Parser &message)
{
    (void)message;
    reply(client, ":ircserv ERROR :Closing Link\r\n");

    return (false);
}


// Performs the privmsg operation.
bool    ClientCommand::privmsg(Client &client, const Parser &message)
{
    const std::vector<std::string>  &params = message.params();
    Client                          *targetClient;

    if (!client.isRegistered())
    {
        reply(client, ":ircserv 451 " + target(client)
            + " :You have not registered\r\n");

        return (true);
    }

    if (params.empty())
    {
        reply(client, ":ircserv 411 " + target(client)
            + " :No recipient given (PRIVMSG)\r\n");

        return (true);
    }

    if (params.size() < 2 || params[1].empty())
    {
        reply(client, ":ircserv 412 " + target(client)
            + " :No text to send\r\n");

        return (true);
    }

    targetClient = _clients.findByNickname(params[0]);
    if (targetClient == NULL)
        reply(client, ":ircserv 401 " + target(client)
            + " " + params[0] + " :No such nick/channel\r\n");
    else
        targetClient->sendBuffer().append(":" + client.prefix()
            + " PRIVMSG " + targetClient->nickname() + " :"
            + params[1] + "\r\n");

    return (true);
}


// Performs the unknown operation.
bool    ClientCommand::unknown(Client &client, const Parser &message)
{
    reply(client, ":ircserv 421 " + target(client)
        + " " + message.name() + " :Unknown command\r\n");

    return (true);
}


// Queues an IRC reply for a client.
void    ClientCommand::reply(Client &client, const std::string &message) const
{
    client.sendBuffer().append(message);
}


// Returns the nickname used as the reply target.
const std::string   &ClientCommand::target(Client &client) const
{
    static const std::string unknownTarget = "*";

    if (client.hasNickname())
        return (client.nickname());

    return (unknownTarget);
}


// Performs the send registration if ready operation.
void    ClientCommand::sendRegistrationIfReady(Client &client,
    bool wasRegistered)
{
    if (wasRegistered || !client.isRegistered())
        return ;

    reply(client, ":ircserv 001 " + client.nickname()
        + " :Welcome to ircserv\r\n");

    reply(client, ":ircserv 221 " + client.nickname()
        + " +i\r\n");
}
