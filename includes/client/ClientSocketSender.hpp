#ifndef CLIENTSOCKETSENDER_HPP
# define CLIENTSOCKETSENDER_HPP

class ClientManager;

class ClientSocketSender
{
public:
    ClientSocketSender();
    ~ClientSocketSender();

    void    sendPending(ClientManager &, int);

private:
    ClientSocketSender(const ClientSocketSender &);
    ClientSocketSender &operator=(const ClientSocketSender &);
};

#endif
