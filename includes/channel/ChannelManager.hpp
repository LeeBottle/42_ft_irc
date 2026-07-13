#ifndef CHANNELMANAGER_HPP
# define CHANNELMANAGER_HPP

# include <string>
# include <vector>

// Stores one IRC channel, its members, topic, and mode state.
class Channel;
// Stores one connected IRC client and its protocol state.
class Client;

// Owns channels and coordinates channel lookup and removal.
class ChannelManager
{
public:
    // Initializes this object with the supplied state.
    ChannelManager();
    // Destroys this object and releases its owned resources.
    ~ChannelManager();

    // Performs the *find or create operation.
    Channel *findOrCreate(const std::string &);
    // Performs the *find operation.
    Channel *find(const std::string &);
    // Removes empty.
    void    removeEmpty(Channel *);
    // Removes a client from every channel it joined.
    void    removeClientFromAll(Client *);

private:
    std::vector<Channel *> _channels;

    // Initializes this object with the supplied state.
    ChannelManager(const ChannelManager &);
    // Performs the &operator= operation.
    ChannelManager &operator=(const ChannelManager &);

    // Removes all stored data.
    void    clear();
};

#endif
