#ifndef CHANNELMANAGER_HPP
# define CHANNELMANAGER_HPP

# include <string>
# include <vector>

class Channel;
class Client;

// own channel and coordinate channel lookup and removal
class ChannelManager
{
public:
    ChannelManager();
    ~ChannelManager();

    Channel *findOrCreate(const std::string &);
    Channel *find(const std::string &);
    void    removeEmpty(Channel *);
    void    removeClientFromAll(Client *);

private:
    std::vector<Channel *> _channels;

    ChannelManager(const ChannelManager &);
    ChannelManager &operator=(const ChannelManager &);

    void    clear();
};

#endif
