#ifndef COMMANDMODEPREPARE_HPP
# define COMMANDMODEPREPARE_HPP

# include "command/CommandBase.hpp"

class Channel;
class ChannelManager;
class Client;
class ClientManager;
class Parser;

class CommandModePrepare : public CommandBase
{
public:
    CommandModePrepare(ChannelManager &, ClientManager &);
    ~CommandModePrepare();

    bool    shouldStop(Client &, const Parser &, Channel *&);

private:
    CommandModePrepare();
    CommandModePrepare(const CommandModePrepare &);
    CommandModePrepare &operator=(const CommandModePrepare &);

    bool    executeUserMode(Client &, const Parser &);
};

#endif
