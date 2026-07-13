#ifndef MEMBERLIST_HPP
# define MEMBERLIST_HPP

# include <cstddef>
# include <vector>

// Stores one connected IRC client and its protocol state.
class Client;

// Tracks clients that currently belong to a channel.
class MemberList
{
public:
    // Initializes this object with the supplied state.
    MemberList();
    // Destroys this object and releases its owned resources.
    ~MemberList();

    // Performs the &all operation.
    const std::vector<Client *> &all() const;
    
    // Performs the count operation.
    size_t  count() const;
    // Reports whether empty.
    bool    isEmpty() const;
    // Reports whether s.
    bool    has(Client *) const;
    // Adds an item to this collection.
    void    add(Client *);
    // Removes a disconnected client from channels and client storage.
    void    remove(Client *);

private:
    std::vector<Client *>   _members;

    // Initializes this object with the supplied state.
    MemberList(const MemberList &);
    // Performs the &operator= operation.
    MemberList &operator=(const MemberList &);
};

#endif
