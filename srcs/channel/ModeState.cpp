#include "channel/ModeState.hpp"

#include <sstream>


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


ModeState::~ModeState()
{
}


const std::string   &ModeState::topic() const
{
    return (_topic);
}


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


std::string ModeState::modeParameters() const
{
    std::ostringstream stream;

    if (_hasKey)
        stream << " " << _key;

    if (_hasLimit)
        stream << " " << _limit;

    return (stream.str());
}


bool    ModeState::inviteOnly() const
{
    return (_inviteOnly);
}


bool    ModeState::topicRestricted() const
{
    return (_topicRestricted);
}


bool    ModeState::hasKey() const
{
    return (_hasKey);
}


const std::string   &ModeState::key() const
{
    return (_key);
}


bool    ModeState::hasLimit() const
{
    return (_hasLimit);
}


size_t  ModeState::limit() const
{
    return (_limit);
}


void    ModeState::setTopic(const std::string &topic)
{
    _topic = topic;
}


bool    ModeState::setInviteOnly(bool value)
{
    if (_inviteOnly == value)
        return (false);

    _inviteOnly = value;

    return (true);
}


bool    ModeState::setTopicRestricted(bool value)
{
    if (_topicRestricted == value)
        return (false);

    _topicRestricted = value;

    return (true);
}


void    ModeState::setKey(const std::string &key)
{
    _hasKey = true;
    _key = key;
}


bool    ModeState::clearKey()
{
    if (!_hasKey)
        return (false);

    _hasKey = false;
    _key.clear();

    return (true);
}


void    ModeState::setLimit(size_t limit)
{
    _hasLimit = true;
    _limit = limit;
}


bool    ModeState::clearLimit()
{
    if (!_hasLimit)
        return (false);

    _hasLimit = false;
    _limit = 0;

    return (true);
}
