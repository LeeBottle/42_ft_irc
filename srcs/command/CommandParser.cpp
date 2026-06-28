#include "command/CommandParser.hpp"

#include <cctype>

bool    CommandParser::parse(const std::string &line, Command &command)
{
    size_t  index;
    size_t  start;

    index = 0;
    while (index < line.size() && line[index] == ' ')
        ++index;
    if (index == line.size())
        return (false);
    start = index;
    while (index < line.size() && line[index] != ' ')
        ++index;
    command.setName(toUpper(line.substr(start, index - start)));
    while (index < line.size())
    {
        while (index < line.size() && line[index] == ' ')
            ++index;
        if (index == line.size())
            break ;
        if (line[index] == ':')
        {
            command.addParam(line.substr(index + 1));
            break ;
        }
        start = index;
        while (index < line.size() && line[index] != ' ')
            ++index;
        command.addParam(line.substr(start, index - start));
    }
    return (true);
}

std::string CommandParser::toUpper(const std::string &value)
{
    std::string result;
    size_t      index;

    result = value;
    index = 0;
    while (index < result.size())
    {
        result[index] = static_cast<char>(
            std::toupper(static_cast<unsigned char>(result[index])));
        ++index;
    }
    return (result);
}
