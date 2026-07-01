#include "channel/ChannelModeState.hpp"

ChannelModeState::ChannelModeState()
    : _topic(), _inviteOnly(false), _topicRestricted(true), _hasKey(false),
      _key(), _hasLimit(false), _limit(0)
{
}

ChannelModeState::~ChannelModeState()
{
}

const std::string   &ChannelModeState::getTopic() const
{
    return (_topic);
}

std::string ChannelModeState::getModeString() const
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

std::string ChannelModeState::getModeParameters() const
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

bool    ChannelModeState::isInviteOnly() const
{
    return (_inviteOnly);
}

bool    ChannelModeState::isTopicRestricted() const
{
    return (_topicRestricted);
}

bool    ChannelModeState::hasKey() const
{
    return (_hasKey);
}

const std::string   &ChannelModeState::getKey() const
{
    return (_key);
}

bool    ChannelModeState::hasLimit() const
{
    return (_hasLimit);
}

size_t  ChannelModeState::getLimit() const
{
    return (_limit);
}

void    ChannelModeState::setTopic(const std::string &topic)
{
    _topic = topic;
}

bool    ChannelModeState::setInviteOnly(bool value)
{
    if (_inviteOnly == value)
        return (false);
    _inviteOnly = value;
    return (true);
}

bool    ChannelModeState::setTopicRestricted(bool value)
{
    if (_topicRestricted == value)
        return (false);
    _topicRestricted = value;
    return (true);
}

void    ChannelModeState::setKey(const std::string &key)
{
    _hasKey = true;
    _key = key;
}

bool    ChannelModeState::clearKey()
{
    if (!_hasKey)
        return (false);
    _hasKey = false;
    _key.clear();
    return (true);
}

void    ChannelModeState::setLimit(size_t limit)
{
    _hasLimit = true;
    _limit = limit;
}

bool    ChannelModeState::clearLimit()
{
    if (!_hasLimit)
        return (false);
    _hasLimit = false;
    _limit = 0;
    return (true);
}
