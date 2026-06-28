#ifndef COMMANDCHANNELJOIN_HPP
# define COMMANDCHANNELJOIN_HPP

# include "command/CommandChannelBase.hpp"

class Client;
class Command;

class CommandChannelJoin : public CommandChannelBase
{
public:
    CommandChannelJoin(ChannelManager &, ClientManager &);
    ~CommandChannelJoin();

    void    handleJoin(Client &, const Command &);
    void    handlePart(Client &, const Command &);

private:
    CommandChannelJoin();
    CommandChannelJoin(const CommandChannelJoin &);
    CommandChannelJoin &operator=(const CommandChannelJoin &);
};

#endif
