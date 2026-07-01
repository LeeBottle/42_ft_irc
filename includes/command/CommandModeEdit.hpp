#ifndef COMMANDMODEEDIT_HPP
# define COMMANDMODEEDIT_HPP

# include <cstddef>
# include <string>

struct CommandModeEdit
{
    std::string changes;
    std::string params;
    size_t      paramIndex;
    char        sign;
    char        currentSign;

    CommandModeEdit();
    void    addChange(char mode);
};

#endif
