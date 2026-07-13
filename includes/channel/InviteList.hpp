#ifndef INVITELIST_HPP
# define INVITELIST_HPP

# include <vector>

// Stores one connected IRC client and its protocol state.
class Client;

// Tracks clients invited to an invite-only channel.
class InviteList
{
public:
    // Initializes this object with the supplied state.
    InviteList();
    // Destroys this object and releases its owned resources.
    ~InviteList();

    // Reports whether s.
    bool    has(Client *) const;
    // Adds an item to this collection.
    void    add(Client *);
    // Removes a disconnected client from channels and client storage.
    void    remove(Client *);

private:
    std::vector<Client *>   _invites;

    // Initializes this object with the supplied state.
    InviteList(const InviteList &);
    // Performs the &operator= operation.
    InviteList &operator=(const InviteList &);
};

#endif
