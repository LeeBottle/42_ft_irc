#ifndef COMMANDCHANNELMESSAGE_HPP
# define COMMANDCHANNELMESSAGE_HPP

# include "command/CommandChannelBase.hpp"

class Client;
class Command;

class CommandChannelMessage : public CommandChannelBase
{
public:
    CommandChannelMessage(ChannelManager &, ClientManager &);
    ~CommandChannelMessage();

    void    handlePrivmsg(Client &, const Command &);

private:
    CommandChannelMessage();
    CommandChannelMessage(const CommandChannelMessage &);
    CommandChannelMessage &operator=(const CommandChannelMessage &);

    void    sendChannelMessage(Client &, const std::string &, const std::string &);
};

#endif
