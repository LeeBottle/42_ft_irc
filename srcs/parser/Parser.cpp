#include "parser/Parser.hpp"
#include "client/Client.hpp"

#include <cctype>

Parser::Parser()
    : _name(), _params()
{
}

Parser::~Parser()
{
}

const std::string   &Parser::getName() const
{
    return (_name);
}

const std::vector<std::string>  &Parser::getParams() const
{
    return (_params);
}

void    Parser::setName(const std::string &name)
{
    _name = name;
}

void    Parser::addParam(const std::string &param)
{
    _params.push_back(param);
}

bool    Parser::popLine(Client &client, std::string &line)
{
    const std::string   &buffer = client.getReceiveBuffer();
    size_t              delimiter;

    delimiter = buffer.find("\r\n");
    if (delimiter == std::string::npos)
        return (false);
    line = buffer.substr(0, delimiter);
    client.removeReceived(delimiter + 2);
    return (true);
}

bool    Parser::parse(const std::string &line, Parser &message)
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
    message.setName(toUpper(line.substr(start, index - start)));
    while (index < line.size())
    {
        while (index < line.size() && line[index] == ' ')
            ++index;
        if (index == line.size())
            break ;
        if (line[index] == ':')
        {
            message.addParam(line.substr(index + 1));
            break ;
        }
        start = index;
        while (index < line.size() && line[index] != ' ')
            ++index;
        message.addParam(line.substr(start, index - start));
    }
    return (true);
}

std::string Parser::toUpper(const std::string &value)
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
