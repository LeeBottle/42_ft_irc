#include "event/Event.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>


// Initializes this object with the supplied state.
Event::Event()
{
}

// Destroys this object and releases its owned resources.
Event::~Event()
{
}


// Waits until a monitored file descriptor becomes ready.
bool    Event::wait(std::vector<struct pollfd> &pollFds)
{
    int pollResult;

    pollResult = ::poll(&pollFds[0], pollFds.size(), -1);
    if (pollResult == -1)   // if poll() fail, return -1
    {
        if (errno == EINTR)     // interrupted system call
            return (true);

        return (reportSystemError("poll"));
    }

    return (true);
}


// Performs the report system error operation.
bool    Event::reportSystemError(const char *functionName)
{
    std::cerr << functionName << ": " << std::strerror(errno) << std::endl;
    return (false);
}
