#include "server/Signal.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <signal.h>


static volatile sig_atomic_t   g_serverStopRequested = 0;


static void handleStopSignal(int signalNumber)
{
    (void)signalNumber;
    g_serverStopRequested = 1;
}


Signal::Signal()
{
}


Signal::~Signal()
{
}


// set up signal
bool    Signal::setup()
{
    struct sigaction action;
    struct sigaction ignoreAction;

    g_serverStopRequested = 0;
    std::memset(&action, 0, sizeof(action));
    action.sa_handler = handleStopSignal;

    if (::sigemptyset(&action.sa_mask) == -1)
        return (reportSystemError("sigemptyset"));

    if (::sigaction(SIGINT, &action, NULL) == -1)   // cause ctrl+C
        return (reportSystemError("sigaction"));

    if (::sigaction(SIGTERM, &action, NULL) == -1)  // cause recieve kill
        return (reportSystemError("sigaction"));

    std::memset(&ignoreAction, 0, sizeof(ignoreAction));
    ignoreAction.sa_handler = SIG_IGN;      // for ignoring SIGPIPE

    if (::sigemptyset(&ignoreAction.sa_mask) == -1)
        return (reportSystemError("sigemptyset"));

    if (::sigaction(SIGPIPE, &ignoreAction, NULL) == -1)
        return (reportSystemError("sigaction"));

    return (true);
}


// check whether shutdown has been requested
bool    Signal::shouldStop() const
{
    return (g_serverStopRequested != 0);
}


// used command DIE except other signal handler
void    Signal::requestStop()
{
    g_serverStopRequested = 1;
}


bool    Signal::reportSystemError(const char *functionName) const
{
    std::cerr << functionName << ": " << std::strerror(errno) << std::endl;
    return (false);
}
