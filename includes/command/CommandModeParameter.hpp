#ifndef COMMANDMODEPARAMETER_HPP
# define COMMANDMODEPARAMETER_HPP

# include "command/CommandBase.hpp"

# include <cstddef>
# include <string>
# include <vector>

class Channel;
class ChannelManager;
class Client;
class ClientManager;
struct CommandModeEdit;

class CommandModeParameter : public CommandBase
{
public:
    CommandModeParameter(ChannelManager &, ClientManager &);
    ~CommandModeParameter();

    bool    applyKeyMode(Client &, Channel &,
                const std::vector<std::string> &, CommandModeEdit &);
    bool    applyLimitMode(Client &, Channel &,
                const std::vector<std::string> &, CommandModeEdit &);
    void    sendParameterError(Client &);

private:
    CommandModeParameter();
    CommandModeParameter(const CommandModeParameter &);
    CommandModeParameter &operator=(const CommandModeParameter &);

    bool    parseLimit(const std::string &, size_t &) const;
};

#endif
