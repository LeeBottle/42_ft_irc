#include "client/Client.hpp"

#include <unistd.h>


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


Client::~Client()
{
    if (_fd != -1)
    {
        ::close(_fd);
        _fd = -1;
    }
}


int Client::fd() const
{
    return (_fd);
}


const std::string   &Client::nickname() const
{
    return (_nickname);
}


const std::string   &Client::username() const
{
    return (_username);
}


const std::string   &Client::realname() const
{
    return (_realname);
}


std::string Client::prefix() const
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


void    Client::setUser(const std::string &username,
    const std::string &realname)
{
    _username = username;
    _realname = realname;
    updateRegistration();
}


void    Client::appendReceivedData(const char *data, size_t length)
{
    _receive.append(data, length);
}


bool    Client::popReceivedLine(std::string &line)
{
    return (_receive.pop(line));
}


void    Client::queueSendData(const std::string &data)
{
    _send.append(data);
}


bool    Client::hasSendData() const
{
    return (_send.hasData());
}


const char  *Client::sendData() const
{
    return (_send.data());
}


size_t  Client::sendSize() const
{
    return (_send.size());
}


void    Client::removeSentData(size_t length)
{
    _send.remove(length);
}


void    Client::updateRegistration()
{
    if (_hasPassword && hasNickname() && hasUser())
        _registered = true;
}
