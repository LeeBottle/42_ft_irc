#ifndef COMMANDMODEOPERATOR_HPP
# define COMMANDMODEOPERATOR_HPP

# include "command/CommandBase.hpp"

# include <string>
# include <vector>

class Channel;
class ChannelManager;
class Client;
class ClientManager;
struct CommandModeEdit;

class CommandModeOperator : public CommandBase
{
public:
    CommandModeOperator(ChannelManager &, ClientManager &);
    ~CommandModeOperator();

    bool    applyOperatorMode(Client &, Channel &,
                const std::vector<std::string> &, CommandModeEdit &);

private:
    CommandModeOperator();
    CommandModeOperator(const CommandModeOperator &);
    CommandModeOperator &operator=(const CommandModeOperator &);
};

#endif
