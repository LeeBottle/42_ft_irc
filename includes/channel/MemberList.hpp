#ifndef MEMBERLIST_HPP
# define MEMBERLIST_HPP

# include <cstddef>
# include <vector>

class Client;

// track client that currently belong to a channel
class MemberList
{
public:
    MemberList();
    ~MemberList();

    const std::vector<Client *> &all() const;
    
    size_t  count() const;
    bool    isEmpty() const;
    bool    has(Client *) const;
    void    add(Client *);
    void    remove(Client *);

private:
    std::vector<Client *>   _members;

    MemberList(const MemberList &);
    MemberList &operator=(const MemberList &);
};

#endif
