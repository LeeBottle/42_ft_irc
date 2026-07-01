#ifndef COMMANDBASE_HPP
# define COMMANDBASE_HPP

# include <string>

class Channel;
class ChannelManager;
class Client;
class ClientManager;

class CommandBase
{
protected:
    CommandBase(ChannelManager &, ClientManager &);
    ~CommandBase();

    ChannelManager  &_channels;
    ClientManager   &_clients;

    bool    isValidChannelName(const std::string &) const;
    void    sendNamesReply(Client &, Channel &);
    void    sendTopicReply(Client &, Channel &);
    void    queueReply(Client &, const std::string &);

    const std::string   &getReplyTarget(Client &) const;

private:
    CommandBase();
    CommandBase(const CommandBase &);
    CommandBase &operator=(const CommandBase &);
};

#endif
