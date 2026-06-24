#include "Client.hpp"
#include <unistd.h>

Client::Client(int fd) 
    : _fd(fd),
      _receiveBuffer(),
      _sendBuffer()
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

void    Client::appendReceived(const char *data, std::string::size_type length)
{
    _receiveBuffer.append(data, length);
}

bool    Client::extractLine(std::string& line)
{
    std::string::size_type  delimiter;

    delimiter = _receiveBuffer.find("\r\n");
    if (delimiter == std::string::npos)
        return (false);
    line = _receiveBuffer.substr(0, delimiter);
    _receiveBuffer.erase(0, delimiter + 2);
    return (true);
}

void    Client::appendSend(const std::string& message)
{
    _sendBuffer.append(message);
}

bool    Client::hasPendingSend() const
{
    return (!_sendBuffer.empty());
}

const char* Client::getSendData() const
{
    return (_sendBuffer.data());
}

std::string::size_type  Client::getSendSize() const
{
    return (_sendBuffer.size());
}

void    Client::removeSent(std::string::size_type length)
{
    _sendBuffer.erase(0, length);
}
