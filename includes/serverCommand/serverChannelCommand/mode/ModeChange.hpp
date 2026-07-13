#ifndef MODECHANGE_HPP
# define MODECHANGE_HPP

# include <cstddef>
# include <string>
# include <vector>

// Stores one connected IRC client and its protocol state.
class Client;

// Defines the ModeOperation component and its interface.
struct ModeOperation
{
    char        sign;
    char        mode;
    std::string value;
    size_t      limit;
    Client      *target;

    // Initializes this object with the supplied state.
    ModeOperation();
};

// Stores one parsed channel mode change.
struct ModeChange
{
    std::string changes;
    std::string params;
    size_t      paramIndex;
    char        sign;
    char        currentSign;

    std::vector<ModeOperation>  operations;

    // Initializes this object with the supplied state.
    ModeChange();

    // Adds change.
    void    addChange(char);
};

#endif
