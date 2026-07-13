#ifndef EVENT_HPP
# define EVENT_HPP

# include <poll.h>
# include <vector>

// Wraps the blocking poll() wait operation.
class Event
{
public:
    // Initializes this object with the supplied state.
    Event();
    // Destroys this object and releases its owned resources.
    ~Event();

    // Waits until a monitored file descriptor becomes ready.
    bool    wait(std::vector<struct pollfd> &);

private:
    // Initializes this object with the supplied state.
    Event(const Event &);
    // Performs the &operator= operation.
    Event &operator=(const Event &);

    // Performs the report system error operation.
    bool    reportSystemError(const char *);
};

#endif
