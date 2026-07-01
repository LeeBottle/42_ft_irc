#ifndef COMMANDMODEAPPLY_HPP
# define COMMANDMODEAPPLY_HPP

# include "command/CommandBase.hpp"

# include <string>
# include <vector>

class Channel;
class ChannelManager;
class Client;
class ClientManager;
class Parser;
class CommandModeOperator;
class CommandModeParameter;
struct CommandModeEdit;

class CommandModeApply : public CommandBase
{
public:
    CommandModeApply(ChannelManager &, ClientManager &,
        CommandModeParameter &, CommandModeOperator &);
    ~CommandModeApply();

    bool    applyModeString(Client &, Channel &, const Parser &,
                CommandModeEdit &);
    void    broadcastModeChanges(Client &, Channel &,
                const CommandModeEdit &) const;

private:
    CommandModeParameter &_parameter;
    CommandModeOperator  &_operator;

    CommandModeApply();
    CommandModeApply(const CommandModeApply &);
    CommandModeApply &operator=(const CommandModeApply &);

    bool    applyModeLetter(Client &, Channel &,
                const std::vector<std::string> &, CommandModeEdit &,
                char);
    void    applySimpleMode(Channel &, CommandModeEdit &, char) const;
    void    sendBanListEnd(Client &, Channel &);
};

#endif
