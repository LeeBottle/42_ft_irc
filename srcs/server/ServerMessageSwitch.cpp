#include "server/ServerMessageSwitch.hpp"
#include "client/Client.hpp"
#include "parser/Parser.hpp"

ServerMessageSwitch::ServerMessageSwitch(const std::string &password,
    ClientManager &clients, ChannelManager &channels)
    : _commandUser(password, clients), _commandJoinPart(channels, clients),
      _commandMessage(channels, clients), _commandInfo(channels, clients),
      _commandInviteKick(channels, clients), _commandMode(channels, clients)
{
}

ServerMessageSwitch::~ServerMessageSwitch()
{
}

bool    ServerMessageSwitch::branch(Client &client,
    const Parser &message)
{
    if (message.getName() == "CAP")
        _commandUser.executeCap(client, message);
    else if (message.getName() == "PING")
        _commandUser.executePing(client, message);
    else if (message.getName() == "PONG")
        _commandUser.executePong(client, message);
    else if (message.getName() == "QUIT")
    {
        _commandUser.executeQuit(client, message);
        return (false);
    }
    else if (message.getName() == "PASS")
        _commandUser.executePass(client, message);
    else if (message.getName() == "NICK")
        _commandUser.executeNick(client, message);
    else if (message.getName() == "USER")
        _commandUser.executeUser(client, message);
    else if (message.getName() == "JOIN")
        _commandJoinPart.executeJoin(client, message);
    else if (message.getName() == "PART")
        _commandJoinPart.executePart(client, message);
    else if (message.getName() == "PRIVMSG")
        _commandMessage.executePrivmsg(client, message);
    else if (message.getName() == "NAMES")
        _commandInfo.executeNames(client, message);
    else if (message.getName() == "WHO")
        _commandInfo.executeWho(client, message);
    else if (message.getName() == "TOPIC")
        _commandInfo.executeTopic(client, message);
    else if (message.getName() == "INVITE")
        _commandInviteKick.executeInvite(client, message);
    else if (message.getName() == "KICK")
        _commandInviteKick.executeKick(client, message);
    else if (message.getName() == "MODE")
        _commandMode.executeMode(client, message);
    else
        _commandUser.executeUnknown(client, message);
    return (true);
}
