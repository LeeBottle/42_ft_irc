#include "client/Client.hpp"

#include <unistd.h>

Client::Client(int fd)
    : _fd(fd), _hasPassword(false), _registered(false), _nickname(),
    _username(), _realname(), _receiveBuffer(), _sendBuffer()
{
}

Client::~Client()
{
    if (_fd != -1)
    {
        close(_fd);
        _fd = -1;
    }
}

int Client::getFd() const
{
    return (_fd);
}

const std::string   &Client::getNickname() const
{
    return (_nickname);
}

const std::string   &Client::getUsername() const
{
    return (_username);
}

const std::string   &Client::getRealname() const
{
    return (_realname);
}

std::string Client::getPrefix() const
{
    if (_nickname.empty())
        return ("*");
    if (_username.empty())
        return (_nickname + "!" + _nickname + "@localhost");
    return (_nickname + "!" + _username + "@localhost");
}

bool    Client::hasPassword() const
{
    return (_hasPassword);
}

bool    Client::hasNickname() const
{
    return (!_nickname.empty());
}

bool    Client::hasUser() const
{
    return (!_username.empty());
}

bool    Client::isRegistered() const
{
    return (_registered);
}

void    Client::acceptPassword()
{
    _hasPassword = true;
    updateRegistration();
}

void    Client::setNickname(const std::string &nickname)
{
    _nickname = nickname;
    updateRegistration();
}

void    Client::setUser(const std::string &username, const std::string &realname)
{
    _username = username;
    _realname = realname;
    updateRegistration();
}

void    Client::updateRegistration()
{
    if (_hasPassword && hasNickname() && hasUser())
        _registered = true;
}
