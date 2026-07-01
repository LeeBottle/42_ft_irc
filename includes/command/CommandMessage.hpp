#ifndef COMMANDMESSAGE_HPP
# define COMMANDMESSAGE_HPP

# include "command/CommandBase.hpp"

class Client;
class Parser;

class CommandMessage : public CommandBase
{
public:
    CommandMessage(ChannelManager &, ClientManager &);
    ~CommandMessage();

    void    executePrivmsg(Client &, const Parser &);

private:
    CommandMessage();
    CommandMessage(const CommandMessage &);
    CommandMessage &operator=(const CommandMessage &);

    void    sendChannelMessage(Client &, const std::string &, const std::string &);
};

#endif
