#ifndef SENDBUFFER_HPP
# define SENDBUFFER_HPP

# include <cstddef>
# include <string>

// Stores bytes waiting to be sent to a client socket.
class SendBuffer
{
public:
    // Initializes this object with the supplied state.
    SendBuffer();
    // Destroys this object and releases its owned resources.
    ~SendBuffer();

    // Appends new data to the buffer.
    void        append(const std::string &);
    // Reports whether the buffer contains pending data.
    bool        hasData() const;
    // Performs the *data operation.
    const char  *data() const;
    // Returns the number of stored bytes.
    size_t      size() const;
    // Removes a disconnected client from channels and client storage.
    void        remove(size_t);

private:
    std::string _buffer;

    // Initializes this object with the supplied state.
    SendBuffer(const SendBuffer &);
    // Performs the &operator= operation.
    SendBuffer &operator=(const SendBuffer &);
};

#endif
