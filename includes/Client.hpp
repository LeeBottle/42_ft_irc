#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class Client
{
private:
    int         _fd;
    std::string _receiveBuffer;
    std::string _sendBuffer;
    
    Client();
    Client(const Client &);
    Client& operator=(const Client &);

public:
    Client(int);
    ~Client();

    int                     getFd() const;
    void                    appendReceived(const char *, std::string::size_type);
    bool                    extractLine(std::string &);
    void                    appendSend(const std::string &);
    bool                    hasPendingSend() const;
    const char*             getSendData() const;
    std::string::size_type  getSendSize() const;
    void                    removeSent(std::string::size_type);
};

#endif

