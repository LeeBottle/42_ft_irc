#include "server/ClientIO.hpp"
#include "channel/ChannelManager.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"

#include <cerrno>
#include <iostream>
#include <sys/socket.h>


#ifdef DEBUG_RECV
static void printRawByte(unsigned char byte)
{
    if (byte == '\r')
        std::cout << "\\r";
    else if (byte == '\n')
        std::cout << "\\n";
    else
        std::cout << byte;
}

static void printRawLog(int clientFd, char *buffer, ssize_t bytesRead)
{
    ssize_t index;

    std::cout << "[recv raw] fd=" << clientFd << " bytes="
        << bytesRead << " text=[";
    index = 0;
    while (index < bytesRead)
    {
        printRawByte(static_cast<unsigned char>(buffer[index]));
        ++index;
    }
    std::cout << "]" << std::endl;
}
#endif


ClientIO::ClientIO()
{
}


ClientIO::~ClientIO()
{
}


bool    ClientIO::receive(ClientManager &clients, ChannelManager &channels,
    int clientFd)
{
    char    buffer[512];
    ssize_t bytesRead;
    Client  *client;

    client = clients.findByFd(clientFd);
    if (client == NULL)
        return (false);

    bytesRead = ::recv(clientFd, buffer, sizeof(buffer), 0);
    if (bytesRead > 0)
    {
#ifdef DEBUG_RECV
        printRawLog(clientFd, buffer, bytesRead);
#endif
        client->appendReceivedData(buffer,
            static_cast<size_t>(bytesRead));
        return (true);
    }

    if (bytesRead == 0)
    {
        remove(clients, channels, clientFd);
        return (false);
    }

    if (errno == EINTR)
        return (true);

    if (errno == EAGAIN || errno == EWOULDBLOCK)
        return (true);

    remove(clients, channels, clientFd);
    return (false);
}


void    ClientIO::send(ClientManager &clients, ChannelManager &channels,
    int clientFd)
{
    Client  *client;
    ssize_t sendByteCount;

    client = clients.findByFd(clientFd);
    if (client == NULL || !client->hasSendData())
        return ;

    sendByteCount = ::send(clientFd, client->sendData(),
            client->sendSize(), 0);

    if (sendByteCount > 0)
    {
        client->removeSentData(static_cast<size_t>(sendByteCount));
        return ;
    }

    if (sendByteCount == -1 && errno == EINTR)
        return ;

    if (sendByteCount == -1
        && (errno == EAGAIN || errno == EWOULDBLOCK))
        return ;

    remove(clients, channels, clientFd);
}


void    ClientIO::remove(ClientManager &clients, ChannelManager &channels,
    int clientFd)
{
    Client *client;

    client = clients.findByFd(clientFd);
    if (client == NULL)
        return ;

    channels.removeClientFromAll(client);
    clients.removeByFd(clientFd);
}
