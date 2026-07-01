#ifndef COMMANDCHANNELMODEPARAMETER_HPP
# define COMMANDCHANNELMODEPARAMETER_HPP

# include "command/CommandChannelBase.hpp"

# include <cstddef>
# include <string>
# include <vector>

class Channel;
class ChannelManager;
class Client;
class ClientManager;
struct CommandChannelModeEdit;

class CommandChannelModeParameter : public CommandChannelBase
{
public:
    CommandChannelModeParameter(ChannelManager &, ClientManager &);
    ~CommandChannelModeParameter();

    bool    applyKeyMode(Client &, Channel &,
                const std::vector<std::string> &, CommandChannelModeEdit &);
    bool    applyLimitMode(Client &, Channel &,
                const std::vector<std::string> &, CommandChannelModeEdit &);
    void    sendParameterError(Client &);

private:
    CommandChannelModeParameter();
    CommandChannelModeParameter(const CommandChannelModeParameter &);
    CommandChannelModeParameter &operator=(const CommandChannelModeParameter &);

    bool    parseLimit(const std::string &, size_t &) const;
};

#endif
