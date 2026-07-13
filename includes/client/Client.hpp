#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

# include "client/ReceiveBuffer.hpp"
# include "client/SendBuffer.hpp"

// Stores one connected IRC client and its protocol state.
class Client
{
public:
    // Initializes this object with the supplied state.
    Client(int);
    // Destroys this object and releases its owned resources.
    ~Client();

    // Performs the &nickname operation.
    const std::string   &nickname() const;
    // Performs the &username operation.
    const std::string   &username() const;
    // Performs the &realname operation.
    const std::string   &realname() const;
    // Performs the &receive buffer operation.
    ReceiveBuffer       &receiveBuffer();
    // Performs the &send buffer operation.
    SendBuffer          &sendBuffer();
    // Builds the IRC prefix for this client.
    std::string         prefix() const;
    
    // Returns the owned socket file descriptor.
    int     fd() const;
    // Reports whether the password was accepted.
    bool    hasPassword() const;
    // Reports whether the client has a nickname.
    bool    hasNickname() const;
    // Reports whether USER information was received.
    bool    hasUser() const;
    // Reports whether the IRC registration sequence is complete.
    bool    isRegistered() const;
    // Marks the client password as accepted.
    void    acceptPassword();
    // Stores the client nickname.
    void    setNickname(const std::string &);
    // Stores the client username and real name.
    void    setUser(const std::string &, const std::string &);

private:
    int             _fd;
    bool            _hasPassword;
    bool            _registered;
    std::string     _nickname;
    std::string     _username;
    std::string     _realname;
    ReceiveBuffer   _receive;
    SendBuffer      _send;

    // Updates registration after PASS, NICK, and USER state changes.
    void    updateRegistration();

    // Initializes this object with the supplied state.
    Client();
    // Initializes this object with the supplied state.
    Client(const Client &);
    // Performs the &operator= operation.
    Client &operator=(const Client &);
};

#endif
