#ifndef MESSAGE_HANDLER_HPP
# define MESSAGE_HANDLER_HPP

# include "Message.hpp"
# include "Channel.hpp"

# include <string>
# include <vector>

class Client;
class Connection;

class MessageHandler
{
public:
    MessageHandler();
    ~MessageHandler();

    void    receiveClient(Connection &, int);
    void    sendToClient(Connection &, int);
    void    removeClientFromChannels(Client &);
    void    deleteChannelIfEmpty(Channel *);

private:
    std::vector<Channel *> _channels;

    MessageHandler(const MessageHandler &);
    MessageHandler& operator=(const MessageHandler &);

    void    processReceivedLines(Connection &, Client &);
    void    handleMessage(Connection &, Client &, const Message &);
    void    handlePass(Connection &, Client &, const Message &);
    void    handleNick(Connection &, Client &, const Message &);
    void    handleUser(Connection &, Client &, const Message &);
    void    handlePrivmsg(Connection &, Client &, const Message &);
    void    handleJoin(Connection &, Client &, const Message &);
    void    handlePart(Connection &, Client &, const Message &);
    void    handleTopic(Connection &, Client &, const Message &);
    void    handleInvite(Connection &, Client &, const Message &);
    void    handleKick(Connection &, Client &, const Message &);
    void    handleMode(Connection &, Client &, const Message &);
    void    handleWho(Connection &, Client &, const Message &);
    void    handleCap(Connection &, Client &, const Message &);
    void    handlePing(Connection &, Client &, const Message &);
    void    handleQuit(Connection &, Client &, const Message &);
    void    tryRegister(Connection &, Client &);
    void    sendReply(Connection &, Client &, const std::string &);
    bool    isNicknameInUse(Connection &, const Client &,
                const std::string &) const;
    Client  *findClientByNickname(Connection &,
                const std::string &) const;
    Channel *findChannel(const std::string &) const;
    Channel *getOrCreateChannel(const std::string &);
    bool    isChannelName(const std::string &) const;
    void    sendToChannel(Connection &, Channel *,
                const std::string &, Client *);
    void    sendNamesReply(Connection &, Client &, Channel *);
    void    sendTopicReply(Connection &, Client &, Channel *);
    void    sendChannelModeReply(Connection &, Client &, Channel *);
    std::string makeNamesList(Channel *) const;
    bool    parseLimit(const std::string &, unsigned int &) const;
    void    addModeChange(std::string &, char &, char, char) const;
    bool    isValidNickname(const std::string &) const;
    std::string getReplyTarget(const Client &) const;
    std::string makeClientPrefix(const Client &) const;
    std::string toUpperCommand(const std::string &) const;
};

#endif
