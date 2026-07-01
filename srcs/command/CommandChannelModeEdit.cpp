#include "command/CommandChannelModeEdit.hpp"

CommandChannelModeEdit::CommandChannelModeEdit()
    : changes(), params(), paramIndex(2), sign('+'), currentSign('\0')
{
}

void    CommandChannelModeEdit::addChange(char mode)
{
    if (currentSign != sign)
    {
        changes += sign;
        currentSign = sign;
    }
    changes += mode;
}
