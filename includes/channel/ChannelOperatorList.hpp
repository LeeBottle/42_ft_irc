#ifndef CHANNELOPERATORLIST_HPP
# define CHANNELOPERATORLIST_HPP

# include <vector>

class Client;

class ChannelOperatorList
{
public:
    ChannelOperatorList();
    ~ChannelOperatorList();

    bool    hasOperator(Client *) const;
    bool    addOperator(Client *);
    bool    removeOperator(Client *);

private:
    std::vector<Client *>   _operators;

    ChannelOperatorList(const ChannelOperatorList &);
    ChannelOperatorList &operator=(const ChannelOperatorList &);
};

#endif
