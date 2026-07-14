#ifndef EVENT_HPP
# define EVENT_HPP

# include <poll.h>
# include <vector>

// wait for event on non-blocking fd using poll
class Event
{
public:
    Event();
    ~Event();

    bool    wait(std::vector<struct pollfd> &);

private:
    Event(const Event &);
    Event &operator=(const Event &);

    bool    reportSystemError(const char *);
};

#endif
