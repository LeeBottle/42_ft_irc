#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class Client
{
private:
    int         _fd;
    std::string _receiveBuffer;
    std::string _sendBuffer;
    bool        _hasPassword;
    bool        _registered;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    
    Client();
    Client(const Client &);
    Client& operator=(const Client &);

public:
    Client(int);
    ~Client();

    int getFd() const;
    bool    hasPassword() const;
    bool    hasNickname() const;
    bool    hasUsername() const;
    bool    isRegistered() const;
    const std::string   &getNickname() const;
    const std::string   &getUsername() const;
    const std::string   &getRealname() const;
    void    setPasswordAccepted();
    void    setNickname(const std::string &);
    void    setUser(const std::string &, const std::string &);
    void    setRegistered();
    void    appendReceived(const char *, std::string::size_type);
    bool    extractLine(std::string &);
    void    appendSend(const std::string &);
    bool    hasPendingSend() const;
    const char  *getSendData() const;
    std::string::size_type getSendSize() const;
    void    removeSent(std::string::size_type);
};

#endif
