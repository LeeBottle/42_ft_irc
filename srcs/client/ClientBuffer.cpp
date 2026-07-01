#include "client/Client.hpp"

void    Client::appendReceived(const char *data, size_t length)
{
    _receiveBuffer.append(data, length);
}

const std::string   &Client::getReceiveBuffer() const
{
    return (_receiveBuffer);
}

void    Client::removeReceived(size_t length)
{
    _receiveBuffer.erase(0, length);
}

void    Client::queueSend(const std::string &message)
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

size_t  Client::getSendSize() const
{
    return (_sendBuffer.size());
}

void    Client::removeSent(size_t length)
{
    _sendBuffer.erase(0, length);
}
