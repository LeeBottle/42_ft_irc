#ifndef COMMANDINVITEKICK_HPP
# define COMMANDINVITEKICK_HPP

# include "command/CommandBase.hpp"

class Client;
class Parser;

class CommandInviteKick : public CommandBase
{
public:
    CommandInviteKick(ChannelManager &, ClientManager &);
    ~CommandInviteKick();

    void    executeInvite(Client &, const Parser &);
    void    executeKick(Client &, const Parser &);

private:
    CommandInviteKick();
    CommandInviteKick(const CommandInviteKick &);
    CommandInviteKick &operator=(const CommandInviteKick &);
};

#endif
