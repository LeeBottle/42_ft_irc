#ifndef MODEAPPLIER_HPP
# define MODEAPPLIER_HPP

# include "serverCommand/serverChannelCommand/mode/ModeChange.hpp"

class Channel;

// apply validated channel mode change
class ModeApplier
{
public:
    ModeApplier();
    ~ModeApplier();

    void    apply(Channel &, const ModeChange &);

private:
    ModeApplier(const ModeApplier &);
    ModeApplier &operator=(const ModeApplier &);
};

#endif
