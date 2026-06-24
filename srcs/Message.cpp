#include "Message.hpp"

Message::Message()
    : _raw(),
      _prefix(),
      _command(),
      _parameters()
{
}

bool    Message::parse(const std::string& line)
{
    std::string::size_type  position = 0;
    std::string::size_type  nextSpace;

    _raw.clear();
    _prefix.clear();
    _command.clear();
    _parameters.clear();

    if (line.empty())
        return (false);
    if (line[position] == ':')
    {
        nextSpace = line.find(' ', position);
        if (nextSpace == std::string::npos || nextSpace == position + 1)
            return (false);
        _prefix = line.substr(position + 1, nextSpace - position - 1);
        position = nextSpace + 1;
    }
    
    while (position < line.size() && line[position] == ' ')
        ++position;
    
    nextSpace = line.find(' ', position);
    _command = line.substr(position, nextSpace - position);
    if (_command.empty())
        return (false);

    while (nextSpace != std::string::npos)
    {
        position = nextSpace + 1;
        while (position < line.size() && line[position] == ' ')
            ++position;
        if (position == line.size())
            break ;
        if (line[position] == ':')
        {
            _parameters.push_back(line.substr(position + 1));
            break ;
        }
        nextSpace = line.find(' ', position);
        _parameters.push_back(line.substr(position, nextSpace - position));
    }
    _raw = line;

    return (true);
}

const std::string&  Message::getRaw() const
{
    return (_raw);
}

const std::string&  Message::getPrefix() const
{
    return (_prefix);
}

const std::string&  Message::getCommand() const
{
    return (_command);
}

const std::vector<std::string>& Message::getParameters() const
{
    return (_parameters);
}

