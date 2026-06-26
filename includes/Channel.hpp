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
    std::vector<Client *>   _operators;
    std::vector<Client *>   _invitedClients;
    std::string             _topic;
    bool                    _inviteOnly;
    bool                    _topicRestricted;
    bool                    _hasKey;
    std::string             _key;
    bool                    _hasLimit;
    unsigned int            _limit;

    Channel();
    Channel(const Channel &);
    Channel &operator=(const Channel &);

public:
    Channel(const std::string &);
    ~Channel();

    const std::string           &getName() const;
    const std::vector<Client *> &getMembers() const;
    const std::string           &getTopic() const;
    const std::string           &getKey() const;
    unsigned int                getLimit() const;

    bool    hasMember(Client *) const;
    bool    hasOperator(Client *) const;
    bool    hasInvitation(Client *) const;
    bool    hasTopic() const;
    bool    isInviteOnly() const;
    bool    isTopicRestricted() const;
    bool    hasKey() const;
    bool    hasLimit() const;
    void    addMember(Client *);
    void    removeMember(Client *);
    void    addOperator(Client *);
    void    removeOperator(Client *);
    void    addInvitation(Client *);
    void    removeInvitation(Client *);
    void    setTopic(const std::string &);
    void    setInviteOnly(bool);
    void    setTopicRestricted(bool);
    void    setKey(const std::string &);
    void    clearKey();
    void    setLimit(unsigned int);
    void    clearLimit();
    bool    isEmpty() const;
};

#endif
