#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

class Client;

class Channel
{
private:
    std::string             _name;
    std::vector<Client *>   _members;

    Channel();
    Channel(const Channel &);
    Channel &operator=(const Channel &);

public:
    Channel(const std::string &);
    ~Channel();

    const std::string           &getName() const;
    const std::vector<Client *> &getMembers() const;

    bool    hasMember(Client *) const;
    void    addMember(Client *);
    void    removeMember(Client *);
    bool    isEmpty() const;
};

#endif
