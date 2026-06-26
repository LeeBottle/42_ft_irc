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
    std::string             _topic;

    Channel();
    Channel(const Channel &);
    Channel &operator=(const Channel &);

public:
    Channel(const std::string &);
    ~Channel();

    const std::string           &getName() const;
    const std::vector<Client *> &getMembers() const;
    const std::string           &getTopic() const;

    bool    hasMember(Client *) const;
    bool    hasTopic() const;
    void    addMember(Client *);
    void    removeMember(Client *);
    void    setTopic(const std::string &);
    bool    isEmpty() const;
};

#endif
