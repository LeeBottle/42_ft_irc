#ifndef COMMANDPARSER_HPP
# define COMMANDPARSER_HPP

# include <string>

# include "command/Command.hpp"

class CommandParser
{
public:
    static bool parse(const std::string &, Command &);

private:
    CommandParser();
    CommandParser(const CommandParser &);
    CommandParser &operator=(const CommandParser &);

    static std::string  toUpper(const std::string &);
};

#endif
