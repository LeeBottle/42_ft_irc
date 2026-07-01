#ifndef COMMANDCHANNELMODEEDIT_HPP
# define COMMANDCHANNELMODEEDIT_HPP

# include <cstddef>
# include <string>

struct CommandChannelModeEdit
{
    std::string changes;
    std::string params;
    size_t      paramIndex;
    char        sign;
    char        currentSign;

    CommandChannelModeEdit();
    void    addChange(char mode);
};

#endif
