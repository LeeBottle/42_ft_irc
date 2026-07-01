#include "client/ClientSocketReceiver.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"

#include <cerrno>
#include <sys/socket.h>

ClientSocketReceiver::ClientSocketReceiver()
{
}

ClientSocketReceiver::~ClientSocketReceiver()
{
}

bool    ClientSocketReceiver::receive(ClientManager &clients, int clientFd)
{
    char    buffer[512];
    ssize_t bytesRead;
    Client  *client;

    client = clients.find(clientFd);
    if (client == NULL)
        return (false);
    while (true)
    {
        bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
        if (bytesRead > 0)
        {
            client->appendReceived(buffer, static_cast<size_t>(bytesRead));
            continue ;
        }
        if (bytesRead == 0)
        {
            clients.remove(clientFd);
            return (false);
        }
        if (errno == EINTR)
            continue ;
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            clients.remove(clientFd);
            return (false);
        }
        return (true);
    }
}
