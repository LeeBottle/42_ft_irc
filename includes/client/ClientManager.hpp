#ifndef CLIENTMANAGER_HPP
# define CLIENTMANAGER_HPP

# include <string>
# include <vector>

class Client;

class ClientManager
{
public:
    ClientManager();
    ~ClientManager();

    Client  *add(int clientFd);
    Client  *findByFd(int clientFd);
    Client  *findByNickname(const std::string &nickname);
    const std::vector<Client *> &clients() const;
    void    removeByFd(int clientFd);
    void    clear();
    bool    isNicknameInUse(const std::string &, Client &) const;

private:
    std::vector<Client *>   _clients;

    ClientManager(const ClientManager &other);
    ClientManager &operator=(const ClientManager &other);
};

#endif
