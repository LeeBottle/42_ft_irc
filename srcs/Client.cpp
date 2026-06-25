#include "Client.hpp"
#include <unistd.h>

Client::Client(int fd) 
    : _fd(fd), _receiveBuffer(), _sendBuffer(), _hasPassword(false),
    _registered(false), _nickname(), _username(), _realname()
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

bool    Client::hasPassword() const
{
    return (_hasPassword);
}

bool    Client::hasNickname() const
{
    return (!_nickname.empty());
}

bool    Client::hasUsername() const
{
    return (!_username.empty());
}

bool    Client::isRegistered() const
{
    return (_registered);
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

void    Client::setPasswordAccepted()
{
    _hasPassword = true;
}

void    Client::setNickname(const std::string &nickname)
{
    _nickname = nickname;
}

void    Client::setUser(const std::string &username,
    const std::string &realname)
{
    _username = username;
    _realname = realname;
}

void    Client::setRegistered()
{
    _registered = true;
}

void    Client::appendReceived(const char *data, std::string::size_type length)
{
    _receiveBuffer.append(data, length);
}

bool    Client::extractLine(std::string &line)
{
    std::string::size_type delimiter;

    delimiter = _receiveBuffer.find("\r\n");
    if (delimiter == std::string::npos)
        return (false);
    line = _receiveBuffer.substr(0, delimiter);
    _receiveBuffer.erase(0, delimiter + 2);
    return (true);
}

void    Client::appendSend(const std::string &message)
{
    _sendBuffer.append(message);
}

bool    Client::hasPendingSend() const
{
    return (!_sendBuffer.empty());
}

const char  *Client::getSendData() const
{
    return (_sendBuffer.data());
}

std::string::size_type Client::getSendSize() const
{
    return (_sendBuffer.size());
}

void    Client::removeSent(std::string::size_type length)
{
    _sendBuffer.erase(0, length);
}
