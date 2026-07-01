#ifndef COMMANDMODE_HPP
# define COMMANDMODE_HPP

# include "command/CommandBase.hpp"
# include "command/CommandModeApply.hpp"
# include "command/CommandModeOperator.hpp"
# include "command/CommandModeParameter.hpp"
# include "command/CommandModePrepare.hpp"

class Client;
class Parser;

class CommandMode : public CommandBase
{
public:
    CommandMode(ChannelManager &, ClientManager &);
    ~CommandMode();

    void    executeMode(Client &, const Parser &);

private:
    CommandMode();
    CommandMode(const CommandMode &);
    CommandMode &operator=(const CommandMode &);

    CommandModePrepare   _prepare;
    CommandModeParameter _parameter;
    CommandModeOperator  _operator;
    CommandModeApply     _apply;
};

#endif
