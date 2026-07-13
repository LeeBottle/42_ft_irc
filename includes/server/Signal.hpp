#ifndef SIGNAL_HPP
# define SIGNAL_HPP

// install signal handler and store shutdown request state
class Signal
{
public:
    Signal();
    ~Signal();

    bool    setup();
    bool    shouldStop() const;
    void    requestStop();

private:
    Signal(const Signal &);
    Signal &operator=(const Signal &);

    bool    reportSystemError(const char *) const;
};

#endif
