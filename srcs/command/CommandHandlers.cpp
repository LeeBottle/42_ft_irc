#include "command/CommandHandlers.hpp"

CommandHandlers::CommandHandlers(const std::string &password,
    ClientManager &clients, ChannelManager &channels)
    : _user(password, clients), _channelJoin(channels, clients),
      _channelMessage(channels, clients), _channelInfo(channels, clients),
      _channelOperator(channels, clients), _channelMode(channels, clients)
{
}

CommandHandlers::~CommandHandlers()
{
}

CommandUserHandler   &CommandHandlers::user()
{
    return (_user);
}

CommandChannelJoin   &CommandHandlers::channelJoin()
{
    return (_channelJoin);
}

CommandChannelMessage    &CommandHandlers::channelMessage()
{
    return (_channelMessage);
}

CommandChannelInfo   &CommandHandlers::channelInfo()
{
    return (_channelInfo);
}

CommandChannelOperator   &CommandHandlers::channelOperator()
{
    return (_channelOperator);
}

CommandChannelMode   &CommandHandlers::channelMode()
{
    return (_channelMode);
}
