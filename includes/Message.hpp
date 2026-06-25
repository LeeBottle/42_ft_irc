#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>
# include <vector>

class Message
{
private:
    std::string                 _raw;
    std::string                 _prefix;
    std::string                 _command;
    std::vector<std::string>    _parameters;

public:
    Message();

    bool    parse(const std::string &);

    const std::string   &getRaw() const;
    const std::string   &getPrefix() const;
    const std::string   &getCommand() const;
    const std::vector<std::string> &getParameters() const;
};

#endif
