#ifndef COMMANDCHANNELMODE_HPP
# define COMMANDCHANNELMODE_HPP

# include "command/CommandChannelBase.hpp"

# include <cstddef>

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

    bool    parseLimit(const std::string &, size_t &) const;
    void    addModeChange(std::string &, char &, char, char) const;
};

#endif
