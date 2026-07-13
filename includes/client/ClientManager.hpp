#ifndef CLIENTMANAGER_HPP
# define CLIENTMANAGER_HPP

# include <string>
# include <vector>

// Stores one connected IRC client and its protocol state.
class Client;

// Owns connected clients and provides client lookup operations.
class ClientManager
{
public:
    // Initializes this object with the supplied state.
    ClientManager();
    // Destroys this object and releases its owned resources.
    ~ClientManager();

    // Performs the &clients operation.
    const std::vector<Client *> &clients() const;

    // Performs the *add operation.
    Client  *add(int);
    // Performs the *find by fd operation.
    Client  *findByFd(int);
    // Performs the *find by nickname operation.
    Client  *findByNickname(const std::string &);

    // Removes and destroys a client identified by its file descriptor.
    void    removeByFd(int);
    // Removes all stored data.
    void    clear();
    // Reports whether nickname in use.
    bool    isNicknameInUse(const std::string &, Client &) const;

private:
    std::vector<Client *>   _clients;

    // Initializes this object with the supplied state.
    ClientManager(const ClientManager &);
    // Performs the &operator= operation.
    ClientManager &operator=(const ClientManager &);
};

#endif
