#include "event/Event.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>


Event::Event()
{
}


Event::~Event()
{
}


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


bool    Event::reportSystemError(const char *functionName)
{
    std::cerr << functionName << ": " << std::strerror(errno) << std::endl;
    return (false);
}
