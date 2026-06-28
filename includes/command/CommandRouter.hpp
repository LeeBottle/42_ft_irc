#ifndef COMMANDROUTER_HPP
# define COMMANDROUTER_HPP

class Client;
class Command;
class CommandHandlers;

class CommandRouter
{
public:
    CommandRouter();
    ~CommandRouter();

    void    route(CommandHandlers &, Client &, const Command &);

private:
    CommandRouter(const CommandRouter &);
    CommandRouter &operator=(const CommandRouter &);
};

#endif
