#ifndef CLIENTPOLLEVENTHANDLER_HPP
# define CLIENTPOLLEVENTHANDLER_HPP

class ClientManager;

class ClientPollEventHandler
{
public:
    ClientPollEventHandler();
    ~ClientPollEventHandler();

    bool    handleEvent(ClientManager &, int, short, bool &);

private:
    ClientPollEventHandler(const ClientPollEventHandler &);
    ClientPollEventHandler &operator=(const ClientPollEventHandler &);
};

#endif
