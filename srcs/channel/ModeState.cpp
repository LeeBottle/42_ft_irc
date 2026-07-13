#include "channel/ModeState.hpp"


// Initializes this object with the supplied state.
ModeState::ModeState()
    : _topic(),
      _inviteOnly(false),
      _topicRestricted(true),
      _hasKey(false),
      _key(),
      _hasLimit(false),
      _limit(0)
{
}


// Destroys this object and releases its owned resources.
ModeState::~ModeState()
{
}


// Performs the topic operation.
const std::string   &ModeState::topic() const
{
    return (_topic);
}


// Performs the mode string operation.
std::string ModeState::modeString() const
{
    std::string modes;

    modes = "+";

    if (_inviteOnly)
        modes += "i";

    if (_topicRestricted)
        modes += "t";

    if (_hasKey)
        modes += "k";

    if (_hasLimit)
        modes += "l";

    if (modes == "+")
        return ("+");

    return (modes);
}


// Performs the mode parameters operation.
std::string ModeState::modeParameters() const
{
    std::string params;
    size_t      value;
    char        digits[32];
    size_t      index;

    if (_hasKey)
        params += " " + _key;

    if (_hasLimit)
    {
        value = _limit;
        index = sizeof(digits);
        digits[--index] = '\0';

        if (value == 0)
            digits[--index] = '0';

        while (value > 0)
        {
            digits[--index] = static_cast<char>('0' + (value % 10));
            value /= 10;
        }
        params += " ";
        params += &digits[index];
    }

    return (params);
}


// Performs the invite only operation.
bool    ModeState::inviteOnly() const
{
    return (_inviteOnly);
}


// Performs the topic restricted operation.
bool    ModeState::topicRestricted() const
{
    return (_topicRestricted);
}


// Reports whether s key.
bool    ModeState::hasKey() const
{
    return (_hasKey);
}


// Performs the key operation.
const std::string   &ModeState::key() const
{
    return (_key);
}


// Reports whether s limit.
bool    ModeState::hasLimit() const
{
    return (_hasLimit);
}


// Performs the limit operation.
size_t  ModeState::limit() const
{
    return (_limit);
}


// Updates topic.
void    ModeState::setTopic(const std::string &topic)
{
    _topic = topic;
}


// Updates invite only.
bool    ModeState::setInviteOnly(bool value)
{
    if (_inviteOnly == value)
        return (false);

    _inviteOnly = value;

    return (true);
}


// Updates topic restricted.
bool    ModeState::setTopicRestricted(bool value)
{
    if (_topicRestricted == value)
        return (false);

    _topicRestricted = value;

    return (true);
}


// Updates key.
void    ModeState::setKey(const std::string &key)
{
    _hasKey = true;
    _key = key;
}


// Performs the clear key operation.
bool    ModeState::clearKey()
{
    if (!_hasKey)
        return (false);

    _hasKey = false;
    _key.clear();

    return (true);
}


// Updates limit.
void    ModeState::setLimit(size_t limit)
{
    _hasLimit = true;
    _limit = limit;
}


// Performs the clear limit operation.
bool    ModeState::clearLimit()
{
    if (!_hasLimit)
        return (false);

    _hasLimit = false;
    _limit = 0;

    return (true);
}
