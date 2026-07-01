#ifndef COMMANDCHANNELMODEOPERATOR_HPP
# define COMMANDCHANNELMODEOPERATOR_HPP

# include "command/CommandChannelBase.hpp"

# include <string>
# include <vector>

class Channel;
class ChannelManager;
class Client;
class ClientManager;
struct CommandChannelModeEdit;

class CommandChannelModeOperator : public CommandChannelBase
{
public:
    CommandChannelModeOperator(ChannelManager &, ClientManager &);
    ~CommandChannelModeOperator();

    bool    applyOperatorMode(Client &, Channel &,
                const std::vector<std::string> &, CommandChannelModeEdit &);

private:
    CommandChannelModeOperator();
    CommandChannelModeOperator(const CommandChannelModeOperator &);
    CommandChannelModeOperator &operator=(const CommandChannelModeOperator &);
};

#endif
