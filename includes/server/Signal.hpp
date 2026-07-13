#ifndef SIGNAL_HPP
# define SIGNAL_HPP

// Installs signal handlers and stores the shutdown request state.
class Signal
{
public:
    // Initializes this object with the supplied state.
    Signal();
    // Destroys this object and releases its owned resources.
    ~Signal();

    // Sets up the resources required by this component.
    bool    setup();
    // Reports whether shutdown has been requested.
    bool    shouldStop() const;
    // Requests a graceful server shutdown.
    void    requestStop();

private:
    // Initializes this object with the supplied state.
    Signal(const Signal &);
    // Performs the &operator= operation.
    Signal &operator=(const Signal &);

    // Performs the report system error operation.
    bool    reportSystemError(const char *) const;
};

#endif
