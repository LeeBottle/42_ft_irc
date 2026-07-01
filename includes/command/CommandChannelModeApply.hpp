#ifndef COMMANDCHANNELMODEAPPLY_HPP
# define COMMANDCHANNELMODEAPPLY_HPP

# include "command/CommandChannelBase.hpp"

# include <string>
# include <vector>

class Channel;
class ChannelManager;
class Client;
class ClientManager;
class Command;
class CommandChannelModeOperator;
class CommandChannelModeParameter;
struct CommandChannelModeEdit;

class CommandChannelModeApply : public CommandChannelBase
{
public:
    CommandChannelModeApply(ChannelManager &, ClientManager &,
        CommandChannelModeParameter &, CommandChannelModeOperator &);
    ~CommandChannelModeApply();

    bool    applyModeString(Client &, Channel &, const Command &,
                CommandChannelModeEdit &);
    void    broadcastModeChanges(Client &, Channel &,
                const CommandChannelModeEdit &) const;

private:
    CommandChannelModeParameter &_parameter;
    CommandChannelModeOperator  &_operator;

    CommandChannelModeApply();
    CommandChannelModeApply(const CommandChannelModeApply &);
    CommandChannelModeApply &operator=(const CommandChannelModeApply &);

    bool    applyModeLetter(Client &, Channel &,
                const std::vector<std::string> &, CommandChannelModeEdit &,
                char);
    void    applySimpleMode(Channel &, CommandChannelModeEdit &, char) const;
    void    sendBanListEnd(Client &, Channel &);
};

#endif
