#include "bircd.hpp"

void	env::do_select()
{
  r = select(max + 1, &fd_read, &fd_write, NULL, NULL);
}
