#ifndef MODEPARSER_HPP
# define MODEPARSER_HPP

# include "serverCommand/serverChannelCommand/CommandHelper.hpp"
# include "serverCommand/serverChannelCommand/mode/ModeChange.hpp"

// Stores one IRC channel, its members, topic, and mode state.
class Channel;
// Owns channels and coordinates channel lookup and removal.
class ChannelManager;
// Stores one connected IRC client and its protocol state.
class Client;
// Owns connected clients and provides client lookup operations.
class ClientManager;
// Splits one IRC line into command, parameters, and trailing text.
class Parser;

// Parses mode flags and their required parameters.
class ModeParser
{
public:
    // Initializes this object with the supplied state.
    ModeParser(ClientManager &, ChannelManager &);
    // Destroys this object and releases its owned resources.
    ~ModeParser();

    // Performs the collect operation.
    bool    collect(Client &, Channel &, const Parser &, ModeChange &);

private:
    ClientManager   &_clients;

    // Initializes this object with the supplied state.
    ModeParser();
    // Initializes this object with the supplied state.
    ModeParser(const ModeParser &);
    // Performs the &operator= operation.
    ModeParser &operator=(const ModeParser &);

    // Performs the collect letter operation.
    bool    collectLetter(Client &, Channel &,
                const std::vector<std::string> &, ModeChange &, char);
    // Performs the collect key operation.
    bool    collectKey(Client &, Channel &, 
                const std::vector<std::string> &, ModeChange &);
    // Performs the collect limit operation.
    bool    collectLimit(Client &, Channel &,
                const std::vector<std::string> &, ModeChange &);
    // Performs the collect operator operation.
    bool    collectOperator(Client &, Channel &,
                const std::vector<std::string> &, ModeChange &);
    // Performs the send ban end operation.
    void    sendBanEnd(Client &, Channel &);
    // Performs the parse limit operation.
    bool    parseLimit(const std::string &, size_t &) const;
};

#endif
