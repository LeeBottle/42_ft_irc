#ifndef MODESTATE_HPP
# define MODESTATE_HPP

# include <cstddef>
# include <string>

// Stores the configurable mode values of a channel.
class ModeState
{
public:
    // Initializes this object with the supplied state.
    ModeState();
    // Destroys this object and releases its owned resources.
    ~ModeState();

    // Performs the &topic operation.
    const std::string   &topic() const;
    // Performs the &key operation.
    const std::string   &key() const;
    
    // Performs the mode string operation.
    std::string modeString() const;
    // Performs the mode parameters operation.
    std::string modeParameters() const;
    
    // Performs the invite only operation.
    bool    inviteOnly() const;
    // Performs the topic restricted operation.
    bool    topicRestricted() const;
    // Reports whether s key.
    bool    hasKey() const;
    // Reports whether s limit.
    bool    hasLimit() const;
    // Performs the limit operation.
    size_t  limit() const;
    // Updates topic.
    void    setTopic(const std::string &);
    // Updates invite only.
    bool    setInviteOnly(bool);
    // Updates topic restricted.
    bool    setTopicRestricted(bool);
    // Updates key.
    void    setKey(const std::string &);
    // Performs the clear key operation.
    bool    clearKey();
    // Updates limit.
    void    setLimit(size_t);
    // Performs the clear limit operation.
    bool    clearLimit();

private:
    std::string _topic;
    bool        _inviteOnly;
    bool        _topicRestricted;
    bool        _hasKey;
    std::string _key;
    bool        _hasLimit;
    size_t      _limit;

    // Initializes this object with the supplied state.
    ModeState(const ModeState &);
    // Performs the &operator= operation.
    ModeState &operator=(const ModeState &);
};

#endif
