#ifndef SERVERMESSAGESWITCH_HPP
# define SERVERMESSAGESWITCH_HPP

# include <string>

# include "command/CommandInfo.hpp"
# include "command/CommandJoinPart.hpp"
# include "command/CommandMessage.hpp"
# include "command/CommandMode.hpp"
# include "command/CommandInviteKick.hpp"
# include "command/CommandUser.hpp"

class ChannelManager;
class Client;
class ClientManager;
class Parser;

class ServerMessageSwitch
{
public:
    ServerMessageSwitch(const std::string &, ClientManager &, ChannelManager &);
    ~ServerMessageSwitch();

    bool    branch(Client &, const Parser &);

private:
    CommandUser             _commandUser;
    CommandJoinPart      _commandJoinPart;
    CommandMessage   _commandMessage;
    CommandInfo      _commandInfo;
    CommandInviteKick  _commandInviteKick;
    CommandMode      _commandMode;

    ServerMessageSwitch();
    ServerMessageSwitch(const ServerMessageSwitch &);
    ServerMessageSwitch &operator=(const ServerMessageSwitch &);
};

#endif
