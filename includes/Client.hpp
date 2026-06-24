#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class Client
{
private:
    int         _fd;
    std::string _receiveBuffer;
    
    Client();
    Client(const Client &);
    Client& operator=(const Client &);

public:
    Client(int);
    ~Client();

    int     getFd() const;
    void    appendReceived(const char *, std::string::size_type);
};

#endif

