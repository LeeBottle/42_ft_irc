#include "command/Command.hpp"

Command::Command()
    : _name(), _params()
{
}

Command::~Command()
{
}

const std::string   &Command::getName() const
{
    return (_name);
}

const std::vector<std::string>  &Command::getParams() const
{
    return (_params);
}

void    Command::setName(const std::string &name)
{
    _name = name;
}

void    Command::addParam(const std::string &param)
{
    _params.push_back(param);
}
