#ifndef OPERATORLIST_HPP
# define OPERATORLIST_HPP

# include <vector>

// Stores one connected IRC client and its protocol state.
class Client;

// Tracks channel members with operator privileges.
class OperatorList
{
public:
    // Initializes this object with the supplied state.
    OperatorList();
    // Destroys this object and releases its owned resources.
    ~OperatorList();

    // Reports whether s.
    bool    has(Client *) const;
    // Adds an item to this collection.
    bool    add(Client *);
    // Removes a disconnected client from channels and client storage.
    bool    remove(Client *);

private:
    std::vector<Client *>   _operators;

    // Initializes this object with the supplied state.
    OperatorList(const OperatorList &);
    // Performs the &operator= operation.
    OperatorList &operator=(const OperatorList &);
};

#endif
