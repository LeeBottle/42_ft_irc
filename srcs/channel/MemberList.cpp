#include "channel/MemberList.hpp"
#include "client/Client.hpp"

#include <cstddef>


// Initializes this object with the supplied state.
MemberList::MemberList() : _members()
{
}


// Destroys this object and releases its owned resources.
MemberList::~MemberList()
{
}


// Performs the all operation.
const std::vector<Client *> &MemberList::all() const
{
    return (_members);
}


// Performs the count operation.
size_t  MemberList::count() const
{
    return (_members.size());
}


// Reports whether empty.
bool    MemberList::isEmpty() const
{
    return (_members.empty());
}


// Reports whether s.
bool    MemberList::has(Client *member) const
{
    std::vector<Client *>::const_iterator   it;

    it = _members.begin();
    while (it != _members.end())
    {
        if (*it == member)
            return (true);
        ++it;
    }

    return (false);
}


// Adds an item to this collection.
void    MemberList::add(Client *member)
{
    if (member == NULL || has(member))
        return ;

    _members.push_back(member);
}


// Removes a disconnected client from channels and client storage.
void    MemberList::remove(Client *member)
{
    std::vector<Client *>::iterator it;

    it = _members.begin();
    while (it != _members.end())
    {
        if (*it == member)
        {
            _members.erase(it);
            return ;
        }
        ++it;
    }
}
