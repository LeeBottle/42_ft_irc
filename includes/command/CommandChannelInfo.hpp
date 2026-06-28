#ifndef COMMANDCHANNELINFO_HPP
# define COMMANDCHANNELINFO_HPP

# include "command/CommandChannelBase.hpp"

class Client;
class Command;

class CommandChannelInfo : public CommandChannelBase
{
public:
    CommandChannelInfo(ChannelManager &, ClientManager &);
    ~CommandChannelInfo();

    void    handleNames(Client &, const Command &);
    void    handleWho(Client &, const Command &);
    void    handleTopic(Client &, const Command &);

private:
    CommandChannelInfo();
    CommandChannelInfo(const CommandChannelInfo &);
    CommandChannelInfo &operator=(const CommandChannelInfo &);
};

#endif
