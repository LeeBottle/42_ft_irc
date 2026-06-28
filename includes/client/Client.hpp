#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class Client
{
public:
    Client(int);
    ~Client();

    const std::string   &getNickname() const;
    const std::string   &getUsername() const;
    const std::string   &getRealname() const;

    std::string getPrefix() const;
    const char  *getSendData() const;
    
    int     getFd() const;
    bool    hasPassword() const;
    bool    hasNickname() const;
    bool    hasUser() const;
    bool    isRegistered() const;
    void    acceptPassword();
    void    setNickname(const std::string &);
    void    setUser(const std::string &, const std::string &);
    void    appendReceived(const char *, size_t);
    bool    extractLine(std::string &);
    void    queueSend(const std::string &);
    bool    hasPendingSend() const;
    size_t  getSendSize() const;
    void    removeSent(size_t);

private:
    int         _fd;
    bool        _hasPassword;
    bool        _registered;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _receiveBuffer;
    std::string _sendBuffer;

    void    updateRegistration();

    Client();
    Client(const Client &);
    Client &operator=(const Client &);
};

#endif
