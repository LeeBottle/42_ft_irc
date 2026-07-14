#include "client/ReceiveBuffer.hpp"


ReceiveBuffer::ReceiveBuffer() : _buffer()
{
}


ReceiveBuffer::~ReceiveBuffer()
{
}


void    ReceiveBuffer::append(const char *data, size_t length)
{
    _buffer.append(data, length);
}


const std::string   &ReceiveBuffer::data() const
{
    return (_buffer);
}


bool    ReceiveBuffer::pop(std::string &line)
{
    size_t  delimiter;

    delimiter = _buffer.find("\r\n");
    if (delimiter == std::string::npos)     // could not find
        return (false);

    line = _buffer.substr(0, delimiter);
    _buffer.erase(0, delimiter + 2);      // remvoe '/r/n'

    return (true);
}
