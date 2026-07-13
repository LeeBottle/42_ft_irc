#ifndef SENDBUFFER_HPP
# define SENDBUFFER_HPP

# include <cstddef>
# include <string>

// store byte waiting to be sent to a client socket
class SendBuffer
{
public:
    SendBuffer();
    ~SendBuffer();

    void        append(const std::string &);
    bool        hasData() const;
    const char  *data() const;
    size_t      size() const;
    void        remove(size_t);

private:
    std::string _buffer;

    // Initializes this object with the supplied state.
    SendBuffer(const SendBuffer &);
    // Performs the &operator= operation.
    SendBuffer &operator=(const SendBuffer &);
};

#endif
