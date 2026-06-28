#ifndef CHANNELMEMBERLIST_HPP
# define CHANNELMEMBERLIST_HPP

# include <string>
# include <vector>

class ChannelOperatorList;
class Client;

class ChannelMemberList
{
public:
    ChannelMemberList();
    ~ChannelMemberList();

    const std::vector<Client *> &getClients() const;
    
    size_t      size() const;
    bool        isEmpty() const;
    bool        hasClient(Client *) const;
    std::string getNames(const ChannelOperatorList &) const;
    void        addClient(Client *);
    void        removeClient(Client *);
    void        broadcast(Client &, const std::string &);
    void        broadcastAll(const std::string &);

private:
    std::vector<Client *>   _clients;

    ChannelMemberList(const ChannelMemberList &);
    ChannelMemberList &operator=(const ChannelMemberList &);
};

#endif
