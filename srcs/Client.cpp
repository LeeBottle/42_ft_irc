#include "Client.hpp"
#include <unistd.h>

Client::Client(int fd) 
    : _fd(fd),
      _receiveBuffer()
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
