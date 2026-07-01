#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include <vector>

class Client;

class Parser
{
public:
    Parser();
    ~Parser();

    const std::string               &getName() const;
    const std::vector<std::string>  &getParams() const;

    static bool parse(const std::string &, Parser &);
    static bool popLine(Client &, std::string &);

private:
    std::string                 _name;
    std::vector<std::string>    _params;

    void    setName(const std::string &);
    void    addParam(const std::string &);
    static std::string  toUpper(const std::string &);
};

#endif
