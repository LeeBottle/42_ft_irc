#ifndef COMMANDCHANNELMODE_HPP
# define COMMANDCHANNELMODE_HPP

# include "command/CommandChannelBase.hpp"
# include "command/CommandChannelModeApply.hpp"
# include "command/CommandChannelModeOperator.hpp"
# include "command/CommandChannelModeParameter.hpp"
# include "command/CommandChannelModePrepare.hpp"

class Client;
class Command;

class CommandChannelMode : public CommandChannelBase
{
public:
    CommandChannelMode(ChannelManager &, ClientManager &);
    ~CommandChannelMode();

    void    handleMode(Client &, const Command &);

private:
    CommandChannelMode();
    CommandChannelMode(const CommandChannelMode &);
    CommandChannelMode &operator=(const CommandChannelMode &);

    CommandChannelModePrepare   _prepare;
    CommandChannelModeParameter _parameter;
    CommandChannelModeOperator  _operator;
    CommandChannelModeApply     _apply;
};

#endif
