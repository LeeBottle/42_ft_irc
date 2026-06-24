#ifndef SERVER_MESSAGE_HANDLER_HPP
# define SERVER_MESSAGE_HANDLER_HPP

# include "Message.hpp"

# include <string>

class Client;
class ServerConnection;

class ServerMessageHandler
{
private:
    ServerConnection &_connection;

public:
    ServerMessageHandler(ServerConnection &);

    void    receiveClient(int);
    void    sendToClient(int);

private:
    ServerMessageHandler();
    ServerMessageHandler(const ServerMessageHandler &);
    ServerMessageHandler& operator=(const ServerMessageHandler &);

    void    processReceivedLines(Client &);
    void    broadcastMessage(int, const Message &);
};

#endif
