#include "client/Client.hpp"

void    Client::appendReceived(const char *data, size_t length)
{
    _receiveBuffer.append(data, length);
}

bool    Client::extractLine(std::string &line)
{
    size_t delimiter;

    delimiter = _receiveBuffer.find("\r\n");
    if (delimiter == std::string::npos)
        return (false);
    line = _receiveBuffer.substr(0, delimiter);
    _receiveBuffer.erase(0, delimiter + 2);
    return (true);
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
