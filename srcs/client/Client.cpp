#include "client/Client.hpp"

#include <unistd.h>


// Initializes this object with the supplied state.
Client::Client(int fd)
    : _fd(fd),
      _hasPassword(false),
      _registered(false),
      _nickname(),
      _username(),
      _realname(),
      _receive(),
      _send()
{
}


// Destroys this object and releases its owned resources.
Client::~Client()
{
    if (_fd != -1)
    {
        ::close(_fd);
        _fd = -1;
    }
}


// Returns the owned socket file descriptor.
int Client::fd() const
{
    return (_fd);
}


// Returns the client nickname.
const std::string   &Client::nickname() const
{
    return (_nickname);
}


// Returns the client username.
const std::string   &Client::username() const
{
    return (_username);
}


// Returns the client real name.
const std::string   &Client::realname() const
{
    return (_realname);
}


// Returns the client receive buffer.
ReceiveBuffer &Client::receiveBuffer()
{
    return (_receive);
}


// Returns the client send buffer.
SendBuffer    &Client::sendBuffer()
{
    return (_send);
}


// Builds the IRC prefix for this client.
std::string Client::prefix() const
{
    if (_nickname.empty())
        return ("*");

    if (_username.empty())
        return (_nickname + "!" + _nickname + "@localhost");

    return (_nickname + "!" + _username + "@localhost");
}


// Reports whether the password was accepted.
bool    Client::hasPassword() const
{
    return (_hasPassword);
}


// Reports whether the client has a nickname.
bool    Client::hasNickname() const
{
    return (!_nickname.empty());
}


// Reports whether USER information was received.
bool    Client::hasUser() const
{
    return (!_username.empty());
}


// Reports whether the IRC registration sequence is complete.
bool    Client::isRegistered() const
{
    return (_registered);
}


// Marks the client password as accepted.
void    Client::acceptPassword()
{
    _hasPassword = true;
    updateRegistration();
}


// Stores the client nickname.
void    Client::setNickname(const std::string &nickname)
{
    _nickname = nickname;
    updateRegistration();
}


// Stores the client username and real name.
void    Client::setUser(const std::string &username,
    const std::string &realname)
{
    _username = username;
    _realname = realname;
    updateRegistration();
}


// Updates registration after PASS, NICK, and USER state changes.
void    Client::updateRegistration()
{
    if (_hasPassword && hasNickname() && hasUser())
        _registered = true;
}
