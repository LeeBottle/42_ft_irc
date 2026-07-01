#ifndef COMMANDCHANNELMODEPREPARE_HPP
# define COMMANDCHANNELMODEPREPARE_HPP

# include "command/CommandChannelBase.hpp"

class Channel;
class ChannelManager;
class Client;
class ClientManager;
class Command;

class CommandChannelModePrepare : public CommandChannelBase
{
public:
    CommandChannelModePrepare(ChannelManager &, ClientManager &);
    ~CommandChannelModePrepare();

    bool    shouldStop(Client &, const Command &, Channel *&);

private:
    CommandChannelModePrepare();
    CommandChannelModePrepare(const CommandChannelModePrepare &);
    CommandChannelModePrepare &operator=(const CommandChannelModePrepare &);

    bool    handleUserMode(Client &, const Command &);
};

#endif
