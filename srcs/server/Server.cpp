#include "server/Server.hpp"
#include "client/Client.hpp"
#include "server/Signal.hpp"

#include <iostream>
#include <string>
#include <unistd.h>


Server::Server(int port, const std::string &password)
    : _password(password),
      _clients(),
      _channels(),
      _listener(port),
      _poll(),
      _clientIO(),
      _message(_password, _clients, _channels),
      _event(),
      _signal()
{
}


Server::~Server()
{
}


bool    Server::run()
{
    std::vector<struct pollfd>  pollFds;

    if (!_signal.setup())
        return (false);

    if (!_listener.setup())
        return (false);

    while (!_signal.shouldStop())
    {
        _poll.build(pollFds, _listener, _clients);

        if (!_event.wait(pollFds))
            return (false);

        handlePoll(pollFds);
    }

    std::cout << "server shutting down" << std::endl;

    return (true);
}


void    Server::handlePoll(std::vector<struct pollfd> &pollFds)
{
    size_t index;
    int    fd;
    short  revents;

    index = pollFds.size();
    while (index > 0 && !_signal.shouldStop())
    {
        --index;
        fd = pollFds[index].fd;
        revents = pollFds[index].revents;
        if (revents == 0)
            continue ;

        if (fd == STDIN_FILENO)
        {
            if (revents & POLLIN)
                handleTerminal();
        }
        else if (fd == _listener.fd() && (revents & POLLIN))
            acceptClients();
        else if (fd != _listener.fd())
            handleClient(fd, revents);
    }
}

void    Server::handleClient(int clientFd, short revents)
{
    Client  *client;
    bool    hasReceiveData;

    hasReceiveData = false;
    if (revents & (POLLERR | POLLHUP | POLLNVAL))
    {
        _clientIO.remove(_clients, _channels, clientFd);
        return ;
    }

    // 1. 먼저 수신 데이터가 있다면 커널에서 바이트를 바짝 읽어옵니다.
    if (revents & POLLIN)
    {
        if (!_clientIO.receive(_clients, _channels, clientFd))
            return ;
        hasReceiveData = true;
    }

    client = _clients.findByFd(clientFd);

    // 2. ⚠️ 중요: 수신된 데이터를 파싱하고 명령을 먼저 처리합니다 (순서 격상)
    // 이 과정에서 다른 유저들이나 본인의 SendBuffer에 신규 대기 패킷들이 쌓이게 됩니다.
    if (hasReceiveData && client != NULL)
    {
        if (!_message.process(*client))
        {
            _clientIO.send(_clients, _channels, clientFd);
            _clientIO.remove(_clients, _channels, clientFd);
            return;
        }
    }

    // 3. ⚠️ 중요: 명령 처리가 완전히 끝난 후, 혹은 원래 POLLOUT 이벤트가 왔을 때 
    // 최신 상태의 SendBuffer 내용을 커널 소켓으로 즉시 밀어냅니다.
    if ((revents & POLLOUT) || (client != NULL && client->sendBuffer().hasData()))
    {
        _clientIO.send(_clients, _channels, clientFd);
    }
}


/*
void    Server::handleClient(int clientFd, short revents)
{
    Client  *client;
    bool    hasReceiveData;

    hasReceiveData = false;
    if (revents & (POLLERR | POLLHUP | POLLNVAL))
    {
        _clientIO.remove(_clients, _channels, clientFd);
        return ;
    }

    if (revents & POLLIN)
    {
        if (!_clientIO.receive(_clients, _channels, clientFd))
            return ;

        hasReceiveData = true;
    }

    if (revents & POLLOUT)
        _clientIO.send(_clients, _channels, clientFd);

    client = _clients.findByFd(clientFd);
    if (hasReceiveData && client != NULL && !_message.process(*client))
    {
        _clientIO.send(_clients, _channels, clientFd);
        _clientIO.remove(_clients, _channels, clientFd);
    }
}
*/

void    Server::handleTerminal()
{
    std::string line;

    if (!std::getline(std::cin, line))
        return ;

    if (line == "DIE")
        _signal.requestStop();
}


bool    Server::acceptClients()
{
    int clientFd;

    while (true)
    {
        if (!_listener.acceptClient(clientFd))
            return (false);

        if (clientFd == -1)
            return (true);

        _clients.add(clientFd);
        std::cout << "client connected with fd " << clientFd << std::endl;
    }
}
