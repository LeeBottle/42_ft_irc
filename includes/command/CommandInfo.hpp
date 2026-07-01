#ifndef COMMANDINFO_HPP
# define COMMANDINFO_HPP

# include "command/CommandBase.hpp"

class Client;
class Parser;

class CommandInfo : public CommandBase
{
public:
    CommandInfo(ChannelManager &, ClientManager &);
    ~CommandInfo();

    void    executeNames(Client &, const Parser &);
    void    executeWho(Client &, const Parser &);
    void    executeTopic(Client &, const Parser &);

private:
    CommandInfo();
    CommandInfo(const CommandInfo &);
    CommandInfo &operator=(const CommandInfo &);
};

#endif
