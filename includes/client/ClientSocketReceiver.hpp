#ifndef CLIENTSOCKETRECEIVER_HPP
# define CLIENTSOCKETRECEIVER_HPP

class ClientManager;

class ClientSocketReceiver
{
public:
    ClientSocketReceiver();
    ~ClientSocketReceiver();

    bool    receive(ClientManager &, int);

private:
    ClientSocketReceiver(const ClientSocketReceiver &);
    ClientSocketReceiver &operator=(const ClientSocketReceiver &);
};

#endif
