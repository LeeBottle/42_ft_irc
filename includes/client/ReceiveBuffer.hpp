#ifndef RECEIVEBUFFER_HPP
# define RECEIVEBUFFER_HPP

# include <cstddef>
# include <string>

// accumulate received byte and extract complete IRC line
class ReceiveBuffer
{
public:
    ReceiveBuffer();
    ~ReceiveBuffer();

    void                append(const char *, size_t);
    const std::string   &data() const;
    bool                pop(std::string &);
    void                remove(size_t);

private:
    std::string _buffer;

    // Initializes this object with the supplied state.
    ReceiveBuffer(const ReceiveBuffer &);
    // Performs the &operator= operation.
    ReceiveBuffer &operator=(const ReceiveBuffer &);
};

#endif
