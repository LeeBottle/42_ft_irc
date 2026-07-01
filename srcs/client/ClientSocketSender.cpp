#include "client/ClientSocketSender.hpp"
#include "client/Client.hpp"
#include "client/ClientManager.hpp"

#include <cerrno>
#include <sys/socket.h>

ClientSocketSender::ClientSocketSender()
{
}

ClientSocketSender::~ClientSocketSender()
{
}

void    ClientSocketSender::sendPending(ClientManager &clients, int clientFd)
{
    Client  *client;
    ssize_t bytesSent;

    client = clients.find(clientFd);
    if (client == NULL)
        return ;
    while (client->hasPendingSend())
    {
        bytesSent = send(clientFd, client->getSendData(),
                client->getSendSize(), 0);
        if (bytesSent > 0)
            client->removeSent(static_cast<size_t>(bytesSent));
        else if (bytesSent == -1 && errno == EINTR)
            continue ;
        else if (bytesSent == -1
            && (errno == EAGAIN || errno == EWOULDBLOCK))
            return ;
        else
        {
            clients.remove(clientFd);
            return ;
        }
    }
}
