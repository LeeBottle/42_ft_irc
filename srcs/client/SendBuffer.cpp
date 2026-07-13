#include "client/SendBuffer.hpp"


// Initializes this object with the supplied state.
SendBuffer::SendBuffer() : _buffer()
{
}


// Destroys this object and releases its owned resources.
SendBuffer::~SendBuffer()
{
}


// Appends new data to the buffer.
void    SendBuffer::append(const std::string &message)
{
    _buffer.append(message);
}


// Reports whether the buffer contains pending data.
bool    SendBuffer::hasData() const
{
    return (!_buffer.empty());
}


// Returns a pointer to the first pending byte.
const char  *SendBuffer::data() const
{
    return (_buffer.data());
}


// Returns the number of stored bytes.
size_t  SendBuffer::size() const
{
    return (_buffer.size());
}


// Removes a disconnected client from channels and client storage.
void    SendBuffer::remove(size_t length)
{
    _buffer.erase(0, length);
}
