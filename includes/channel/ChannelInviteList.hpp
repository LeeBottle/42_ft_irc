#ifndef CHANNELINVITELIST_HPP
# define CHANNELINVITELIST_HPP

# include <vector>

class Client;

class ChannelInviteList
{
public:
    ChannelInviteList();
    ~ChannelInviteList();

    bool    hasInvite(Client *) const;
    void    addInvite(Client *);
    void    removeInvite(Client *);

private:
    std::vector<Client *>   _invitedClients;

    ChannelInviteList(const ChannelInviteList &);
    ChannelInviteList &operator=(const ChannelInviteList &);
};

#endif
