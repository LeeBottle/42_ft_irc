#ifndef RECEIVEBUFFER_HPP
# define RECEIVEBUFFER_HPP

# include <cstddef>
# include <string>

// Accumulates received bytes and extracts complete IRC lines.
class ReceiveBuffer
{
public:
    // Initializes this object with the supplied state.
    ReceiveBuffer();
    // Destroys this object and releases its owned resources.
    ~ReceiveBuffer();

    // Appends new data to the buffer.
    void                append(const char *, size_t);
    // Performs the &data operation.
    const std::string   &data() const;
    // Extracts and removes the next complete item from the buffer.
    bool                pop(std::string &);
    // Removes a disconnected client from channels and client storage.
    void                remove(size_t);

private:
    std::string _buffer;

    // Initializes this object with the supplied state.
    ReceiveBuffer(const ReceiveBuffer &);
    // Performs the &operator= operation.
    ReceiveBuffer &operator=(const ReceiveBuffer &);
};

#endif
