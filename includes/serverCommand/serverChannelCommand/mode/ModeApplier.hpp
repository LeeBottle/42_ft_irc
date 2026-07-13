#ifndef MODEAPPLIER_HPP
# define MODEAPPLIER_HPP

# include "serverCommand/serverChannelCommand/mode/ModeChange.hpp"

// Stores one IRC channel, its members, topic, and mode state.
class Channel;

// Applies validated channel mode changes.
class ModeApplier
{
public:
    // Initializes this object with the supplied state.
    ModeApplier();
    // Destroys this object and releases its owned resources.
    ~ModeApplier();

    // Applies .
    void    apply(Channel &, const ModeChange &);

private:
    // Initializes this object with the supplied state.
    ModeApplier(const ModeApplier &);
    // Performs the &operator= operation.
    ModeApplier &operator=(const ModeApplier &);
};

#endif
