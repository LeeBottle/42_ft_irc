#include "command/CommandModeEdit.hpp"

CommandModeEdit::CommandModeEdit()
    : changes(), params(), paramIndex(2), sign('+'), currentSign('\0')
{
}

void    CommandModeEdit::addChange(char mode)
{
    if (currentSign != sign)
    {
        changes += sign;
        currentSign = sign;
    }
    changes += mode;
}
