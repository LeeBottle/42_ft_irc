#include "command/CommandRouter.hpp"
#include "command/Command.hpp"
#include "command/CommandHandlers.hpp"

CommandRouter::CommandRouter()
{
}

CommandRouter::~CommandRouter()
{
}

void    CommandRouter::route(CommandHandlers &handlers, Client &client,
    const Command &command)
{
    if (command.getName() == "CAP")
        handlers.user().handleCap(client, command);
    else if (command.getName() == "PING")
        handlers.user().handlePing(client, command);
    else if (command.getName() == "PONG")
        handlers.user().handlePong(client, command);
    else if (command.getName() == "DIE")
        handlers.user().handleDie(client, command);
    else if (command.getName() == "PASS")
        handlers.user().handlePass(client, command);
    else if (command.getName() == "NICK")
        handlers.user().handleNick(client, command);
    else if (command.getName() == "USER")
        handlers.user().handleUser(client, command);
    else if (command.getName() == "JOIN")
        handlers.channelJoin().handleJoin(client, command);
    else if (command.getName() == "PART")
        handlers.channelJoin().handlePart(client, command);
    else if (command.getName() == "PRIVMSG")
        handlers.channelMessage().handlePrivmsg(client, command);
    else if (command.getName() == "NAMES")
        handlers.channelInfo().handleNames(client, command);
    else if (command.getName() == "WHO")
        handlers.channelInfo().handleWho(client, command);
    else if (command.getName() == "TOPIC")
        handlers.channelInfo().handleTopic(client, command);
    else if (command.getName() == "INVITE")
        handlers.channelOperator().handleInvite(client, command);
    else if (command.getName() == "KICK")
        handlers.channelOperator().handleKick(client, command);
    else if (command.getName() == "MODE")
        handlers.channelMode().handleMode(client, command);
    else
        handlers.user().handleUnknown(client, command);
}
