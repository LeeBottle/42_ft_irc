#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include <vector>

// Splits one IRC line into command, parameters, and trailing text.
class Parser
{
public:
    enum Type
    {
        UNKNOWN,
        CAP,
        PING,
        PONG,
        QUIT,
        PASS,
        NICK,
        USER,
        JOIN,
        PART,
        PRIVMSG,
        NAMES,
        WHO,
        TOPIC,
        INVITE,
        KICK,
        MODE
    };

    // Initializes this object with the supplied state.
    Parser();
    // Destroys this object and releases its owned resources.
    ~Parser();

    // Performs the &name operation.
    const std::string               &name() const;
    // Performs the &params operation.
    const std::vector<std::string>  &params() const;

    // Performs the type operation.
    Type    type() const;

    // Parses one IRC protocol line into command fields.
    bool    parse(const std::string &);

private:
    std::string                 _name;
    std::vector<std::string>    _params;
    Type                        _type;

    // Initializes this object with the supplied state.
    Parser(const Parser &);
    // Performs the &operator= operation.
    Parser &operator=(const Parser &);
};

#endif
