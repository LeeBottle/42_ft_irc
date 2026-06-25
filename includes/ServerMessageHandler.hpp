#ifndef SERVER_MESSAGE_HANDLER_HPP
# define SERVER_MESSAGE_HANDLER_HPP

# include "Message.hpp"

# include <string>

class Client;
class ServerConnection;

class ServerMessageHandler
{
public:
    ServerMessageHandler();

    void    receiveClient(ServerConnection &, int);
    void    sendToClient(ServerConnection &, int);

private:
    ServerMessageHandler(const ServerMessageHandler &);
    ServerMessageHandler& operator=(const ServerMessageHandler &);

    void    processReceivedLines(ServerConnection &, Client &);
    void    handleMessage(ServerConnection &, Client &, const Message &);
    void    handlePass(ServerConnection &, Client &, const Message &);
    void    handleNick(ServerConnection &, Client &, const Message &);
    void    handleUser(ServerConnection &, Client &, const Message &);
    void    handlePrivmsg(ServerConnection &, Client &, const Message &);
    void    handleCap(ServerConnection &, Client &, const Message &);
    void    handlePing(ServerConnection &, Client &, const Message &);
    void    handleQuit(ServerConnection &, Client &, const Message &);
    void    tryRegister(ServerConnection &, Client &);
    void    sendReply(ServerConnection &, Client &, const std::string &);
    bool    isNicknameInUse(ServerConnection &, const Client &,
                const std::string &) const;
    Client  *findClientByNickname(ServerConnection &,
                const std::string &) const;
    bool    isValidNickname(const std::string &) const;
    std::string getReplyTarget(const Client &) const;
    std::string makeClientPrefix(const Client &) const;
    std::string toUpperCommand(const std::string &) const;
};

#endif
