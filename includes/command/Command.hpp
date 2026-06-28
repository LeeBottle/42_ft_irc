#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include <vector>

class Command
{
public:
    Command();
    ~Command();

    const std::string               &getName() const;
    const std::vector<std::string>  &getParams() const;

    void    setName(const std::string &);
    void    addParam(const std::string &);

private:
    std::string                 _name;
    std::vector<std::string>    _params;
};

#endif
