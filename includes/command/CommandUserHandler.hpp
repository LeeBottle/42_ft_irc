#ifndef COMMANDUSERHANDLER_HPP
# define COMMANDUSERHANDLER_HPP

# include <string>

class Client;
class ClientManager;
class Command;

class CommandUserHandler
{
public:
    CommandUserHandler(const std::string &, ClientManager &);
    ~CommandUserHandler();

    void    handlePass(Client &, const Command &);
    void    handleNick(Client &, const Command &);
    void    handleUser(Client &, const Command &);
    void    handleCap(Client &, const Command &);
    void    handlePing(Client &, const Command &);
    void    handlePong(Client &, const Command &);
    void    handleDie(Client &, const Command &);
    void    handleUnknown(Client &, const Command &);

private:
    const std::string   &_password;
    ClientManager       &_clients;

    CommandUserHandler();
    CommandUserHandler(const CommandUserHandler &);
    CommandUserHandler &operator=(const CommandUserHandler &);

    void    sendRegistrationIfReady(Client &, bool);
    void    queueReply(Client &, const std::string &);
    const std::string   &getReplyTarget(Client &) const;
};

#endif
