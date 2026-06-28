#ifndef COMMANDCHANNELOPERATOR_HPP
# define COMMANDCHANNELOPERATOR_HPP

# include "command/CommandChannelBase.hpp"

class Client;
class Command;

class CommandChannelOperator : public CommandChannelBase
{
public:
    CommandChannelOperator(ChannelManager &, ClientManager &);
    ~CommandChannelOperator();

    void    handleInvite(Client &, const Command &);
    void    handleKick(Client &, const Command &);

private:
    CommandChannelOperator();
    CommandChannelOperator(const CommandChannelOperator &);
    CommandChannelOperator &operator=(const CommandChannelOperator &);
};

#endif
