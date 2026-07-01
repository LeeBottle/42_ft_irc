#ifndef CHANNELMANAGER_HPP
# define CHANNELMANAGER_HPP

# include <string>
# include <vector>

class Channel;
class Client;

class ChannelManager
{
public:
    ChannelManager();
    ~ChannelManager();

    Channel *findOrCreate(const std::string &);
    Channel *findChannel(const std::string &);
    void    removeEmptyChannel(Channel *);
    void    removeClientFromAllChannels(Client *);

private:
    std::vector<Channel *> _channels;

    ChannelManager(const ChannelManager &);
    ChannelManager &operator=(const ChannelManager &);

    void    clearAllChannels();
};

#endif
