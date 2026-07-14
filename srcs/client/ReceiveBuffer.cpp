#include "client/ReceiveBuffer.hpp"


ReceiveBuffer::ReceiveBuffer() : _buffer()
{
}


ReceiveBuffer::~ReceiveBuffer()
{
}


// append new data to the buffer
void    ReceiveBuffer::append(const char *data, size_t length)
{
    _buffer.append(data, length);
}


// return a pointer to the first pending byte
const std::string   &ReceiveBuffer::data() const
{
    return (_buffer);
}


// extract next complete item from the buffer
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
