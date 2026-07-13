#include "serverCommand/serverChannelCommand/mode/ModeApplier.hpp"
#include "channel/Channel.hpp"


// Initializes this object with the supplied state.
ModeApplier::ModeApplier()
{
}


// Destroys this object and releases its owned resources.
ModeApplier::~ModeApplier()
{
}


// Applies .
void    ModeApplier::apply(Channel &channel, const ModeChange &change)
{
    std::vector<ModeOperation>::const_iterator  it;

    it = change.operations.begin();
    while (it != change.operations.end())
    {
        if (it->mode == 'i')
            channel.modes().setInviteOnly(it->sign == '+');
        else if (it->mode == 't')
            channel.modes().setTopicRestricted(it->sign == '+');
        else if (it->mode == 'k' && it->sign == '+')
            channel.modes().setKey(it->value);
        else if (it->mode == 'k')
            channel.modes().clearKey();
        else if (it->mode == 'l' && it->sign == '+')
            channel.modes().setLimit(it->limit);
        else if (it->mode == 'l')
            channel.modes().clearLimit();
        else if (it->mode == 'o' && it->sign == '+')
            channel.operators().add(it->target);
        else if (it->mode == 'o')
            channel.operators().remove(it->target);
        ++it;
    }
}
