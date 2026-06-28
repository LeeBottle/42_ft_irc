#ifndef COMMANDCHANNELBASE_HPP
# define COMMANDCHANNELBASE_HPP

# include <string>

class Channel;
class ChannelManager;
class Client;
class ClientManager;

class CommandChannelBase
{
protected:
    CommandChannelBase(ChannelManager &, ClientManager &);
    ~CommandChannelBase();

    ChannelManager  &_channels;
    ClientManager   &_clients;

    bool    isValidChannelName(const std::string &) const;
    void    sendNamesReply(Client &, Channel &);
    void    sendTopicReply(Client &, Channel &);
    void    queueReply(Client &, const std::string &);

    const std::string   &getReplyTarget(Client &) const;

private:
    CommandChannelBase();
    CommandChannelBase(const CommandChannelBase &);
    CommandChannelBase &operator=(const CommandChannelBase &);
};

#endif
