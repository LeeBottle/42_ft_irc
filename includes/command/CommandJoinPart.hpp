#ifndef COMMANDJOINPART_HPP
# define COMMANDJOINPART_HPP

# include "command/CommandBase.hpp"

class Client;
class Parser;

class CommandJoinPart : public CommandBase
{
public:
    CommandJoinPart(ChannelManager &, ClientManager &);
    ~CommandJoinPart();

    void    executeJoin(Client &, const Parser &);
    void    executePart(Client &, const Parser &);

private:
    CommandJoinPart();
    CommandJoinPart(const CommandJoinPart &);
    CommandJoinPart &operator=(const CommandJoinPart &);
};

#endif
