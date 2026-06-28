#ifndef COMMANDHANDLERS_HPP
# define COMMANDHANDLERS_HPP

# include "command/CommandChannelInfo.hpp"
# include "command/CommandChannelJoin.hpp"
# include "command/CommandChannelMessage.hpp"
# include "command/CommandChannelMode.hpp"
# include "command/CommandChannelOperator.hpp"
# include "command/CommandUserHandler.hpp"

# include <string>

class ChannelManager;
class ClientManager;

class CommandHandlers
{
public:
    CommandHandlers(const std::string &, ClientManager &, ChannelManager &);
    ~CommandHandlers();

    CommandUserHandler       &user();
    CommandChannelJoin       &channelJoin();
    CommandChannelMessage    &channelMessage();
    CommandChannelInfo       &channelInfo();
    CommandChannelOperator   &channelOperator();
    CommandChannelMode       &channelMode();

private:
    CommandUserHandler       _user;
    CommandChannelJoin       _channelJoin;
    CommandChannelMessage    _channelMessage;
    CommandChannelInfo       _channelInfo;
    CommandChannelOperator   _channelOperator;
    CommandChannelMode       _channelMode;

    CommandHandlers();
    CommandHandlers(const CommandHandlers &);
    CommandHandlers &operator=(const CommandHandlers &);
};

#endif
