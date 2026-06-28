#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

# include "channel/ChannelInviteList.hpp"
# include "channel/ChannelModeState.hpp"
# include "channel/ChannelMemberList.hpp"
# include "channel/ChannelOperatorList.hpp"

class Client;

class Channel
{
public:
    enum JoinResult
    {
        JOIN_ALLOWED,
        JOIN_INVITE_ONLY,
        JOIN_BAD_KEY,
        JOIN_FULL
    };

    Channel(const std::string &);
    ~Channel();

    const std::string           &getName() const;
    const std::vector<Client *> &getClients() const;

    std::string getMemberNames() const;
    JoinResult  canJoin(Client *, const std::string &) const;

    void    addClient(Client *);
    void    removeClient(Client *);
    bool    hasClient(Client *) const;
    void    inviteClient(Client *);
    bool    isOperator(Client *) const;
    bool    grantOperator(Client *);
    bool    revokeOperator(Client *);
    bool    canSetTopic(Client *) const;
    bool    isEmpty() const;
    void    broadcast(Client &, const std::string &);
    void    broadcastAll(const std::string &);
    
    ChannelModeState        &modes();
    const ChannelModeState  &modes() const;

private:
    std::string         _name;
    ChannelMemberList   _members;
    ChannelOperatorList _operators;
    ChannelInviteList   _invites;
    ChannelModeState    _modes;

    Channel();
    Channel(const Channel &);
    Channel &operator=(const Channel &);
};

#endif
