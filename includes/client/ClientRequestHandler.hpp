#ifndef CLIENTREQUESTHANDLER_HPP
# define CLIENTREQUESTHANDLER_HPP

class Client;
class ClientManager;
class CommandHandlers;
class CommandRouter;

class ClientRequestHandler
{
public:
    ClientRequestHandler();
    ~ClientRequestHandler();

    void    handleEvent(ClientManager &, CommandRouter &, CommandHandlers &,
                int, short);

private:
    ClientRequestHandler(const ClientRequestHandler &);
    ClientRequestHandler &operator=(const ClientRequestHandler &);

    void    receiveClient(ClientManager &, CommandRouter &, CommandHandlers &, 
                int);
    bool    processReceivedLines(CommandRouter &, CommandHandlers &, Client &);
    void    sendToClient(ClientManager &, int);
};

#endif
