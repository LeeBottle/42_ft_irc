#ifndef CLIENTMANAGER_HPP
# define CLIENTMANAGER_HPP

# include <poll.h>
# include <string>
# include <vector>

class ChannelManager;
class Client;

class ClientManager
{
public:
    ClientManager(ChannelManager &channels);
    ~ClientManager();

    Client  *add(int clientFd);
    Client  *find(int clientFd);
    Client  *findByNickname(const std::string &nickname);
    void    remove(int clientFd);
    void    closeAll();
    void    appendPollFds(std::vector<struct pollfd> &pollFds) const;
    bool    isNicknameInUse(const std::string &nickname, Client &owner) const;

private:
    std::vector<Client *>   _clients;
    ChannelManager          &_channels;

    ClientManager();
    ClientManager(const ClientManager &other);
    ClientManager &operator=(const ClientManager &other);
};

#endif
