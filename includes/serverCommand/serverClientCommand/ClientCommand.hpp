#ifndef CLIENTCOMMAND_HPP
# define CLIENTCOMMAND_HPP

# include <string>

// Stores one connected IRC client and its protocol state.
class Client;
// Owns connected clients and provides client lookup operations.
class ClientManager;
// Splits one IRC line into command, parameters, and trailing text.
class Parser;

// Handles registration and client-level IRC commands.
class ClientCommand
{
public:
    // Initializes this object with the supplied state.
    ClientCommand(const std::string &, ClientManager &);
    // Destroys this object and releases its owned resources.
    ~ClientCommand();

    // Performs the cap operation.
    bool    cap(Client &, const Parser &);
    // Performs the ping operation.
    bool    ping(Client &, const Parser &);
    // Performs the pong operation.
    bool    pong(Client &, const Parser &);
    // Performs the quit operation.
    bool    quit(Client &, const Parser &);
    // Performs the pass operation.
    bool    pass(Client &, const Parser &);
    // Performs the nick operation.
    bool    nick(Client &, const Parser &);
    // Performs the user operation.
    bool    user(Client &, const Parser &);
    // Performs the privmsg operation.
    bool    privmsg(Client &, const Parser &);
    // Performs the unknown operation.
    bool    unknown(Client &, const Parser &);

private:
    const std::string   &_password;
    ClientManager       &_clients;

    // Initializes this object with the supplied state.
    ClientCommand();
    // Initializes this object with the supplied state.
    ClientCommand(const ClientCommand &);
    // Performs the &operator= operation.
    ClientCommand &operator=(const ClientCommand &);

    // Performs the &target operation.
    const std::string   &target(Client &) const;
    // Queues an IRC reply for a client.
    void                reply(Client &, const std::string &) const;
    // Performs the send registration if ready operation.
    void                sendRegistrationIfReady(Client &, bool);
};

#endif
