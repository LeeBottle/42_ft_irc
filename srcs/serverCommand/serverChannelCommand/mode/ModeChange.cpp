#include "serverCommand/serverChannelCommand/mode/ModeChange.hpp"

#include <cstddef>


// Initializes this object with the supplied state.
ModeOperation::ModeOperation()
    : sign('+'),
      mode('\0'),
      value(),
      limit(0),
      target(NULL)
{
}


// Initializes this object with the supplied state.
ModeChange::ModeChange()
    : changes(),
      params(),
      paramIndex(2),
      sign('+'),
      currentSign('\0'),
      operations()
{
}


// Adds change.
void    ModeChange::addChange(char mode)
{
    if (currentSign != sign)
    {
        changes += sign;
        currentSign = sign;
    }

    changes += mode;
}
