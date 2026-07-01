#include "client/ClientPollEventHandler.hpp"
#include "client/ClientManager.hpp"
#include "client/ClientSocketReceiver.hpp"
#include "client/ClientSocketSender.hpp"

#include <poll.h>

ClientPollEventHandler::ClientPollEventHandler()
{
}

ClientPollEventHandler::~ClientPollEventHandler()
{
}

bool    ClientPollEventHandler::handleEvent(ClientManager &clients,
    int clientFd, short revents, bool &receivedData)
{
    ClientSocketReceiver    receiver;
    ClientSocketSender      sender;

    receivedData = false;
    if (revents & (POLLERR | POLLHUP | POLLNVAL))
    {
        clients.remove(clientFd);
        return (false);
    }
    if (revents & POLLIN)
    {
        if (!receiver.receive(clients, clientFd))
            return (false);
        receivedData = true;
    }
    if (revents & POLLOUT)
        sender.sendPending(clients, clientFd);
    return (true);
}
