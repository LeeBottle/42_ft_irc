#include "client/ReceiveBuffer.hpp"


// Initializes this object with the supplied state.
ReceiveBuffer::ReceiveBuffer() : _buffer()
{
}


// Destroys this object and releases its owned resources.
ReceiveBuffer::~ReceiveBuffer()
{
}


// Appends new data to the buffer.
void    ReceiveBuffer::append(const char *data, size_t length)
{
    _buffer.append(data, length);
}


// Returns a pointer to the first pending byte.
const std::string   &ReceiveBuffer::data() const
{
    return (_buffer);
}


// Extracts and removes the next complete item from the buffer.
bool    ReceiveBuffer::pop(std::string &line)
{
    size_t  delimiter;

    delimiter = _buffer.find("\r\n");
    if (delimiter == std::string::npos)
        return (false);

    line = _buffer.substr(0, delimiter);
    remove(delimiter + 2);

    return (true);
}


// Removes a disconnected client from channels and client storage.
void    ReceiveBuffer::remove(size_t length)
{
    _buffer.erase(0, length);
}
