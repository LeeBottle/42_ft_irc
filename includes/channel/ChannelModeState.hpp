#ifndef CHANNELMODESTATE_HPP
# define CHANNELMODESTATE_HPP

# include <cstddef>
# include <string>

class ChannelModeState
{
public:
    ChannelModeState();
    ~ChannelModeState();

    const std::string   &getTopic() const;
    const std::string   &getKey() const;
    
    std::string getModeString() const;
    std::string getModeParameters() const;
    
    bool    isInviteOnly() const;
    bool    isTopicRestricted() const;
    bool    hasKey() const;
    bool    hasLimit() const;
    size_t  getLimit() const;
    void    setTopic(const std::string &);
    bool    setInviteOnly(bool);
    bool    setTopicRestricted(bool);
    void    setKey(const std::string &);
    bool    clearKey();
    void    setLimit(size_t);
    bool    clearLimit();

private:
    std::string _topic;
    bool        _inviteOnly;
    bool        _topicRestricted;
    bool        _hasKey;
    std::string _key;
    bool        _hasLimit;
    size_t      _limit;

    ChannelModeState(const ChannelModeState &);
    ChannelModeState &operator=(const ChannelModeState &);
};

#endif
