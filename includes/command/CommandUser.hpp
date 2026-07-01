#ifndef COMMANDUSER_HPP
# define COMMANDUSER_HPP

# include <string>

class Client;
class ClientManager;
class Parser;

class CommandUser
{
public:
    CommandUser(const std::string &, ClientManager &);
    ~CommandUser();

    void    executePass(Client &, const Parser &);
    void    executeNick(Client &, const Parser &);
    void    executeUser(Client &, const Parser &);
    void    executeCap(Client &, const Parser &);
    void    executePing(Client &, const Parser &);
    void    executePong(Client &, const Parser &);
    void    executeQuit(Client &, const Parser &);
    void    executeUnknown(Client &, const Parser &);

private:
    const std::string   &_password;
    ClientManager       &_clients;

    CommandUser();
    CommandUser(const CommandUser &);
    CommandUser &operator=(const CommandUser &);

    void    sendRegistrationIfReady(Client &, bool);
    void    queueReply(Client &, const std::string &);
    const std::string   &getReplyTarget(Client &) const;
};

#endif
