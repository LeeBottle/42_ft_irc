#include "bircd.hpp"

void	env::main_loop()
{
  while (1)
    {
      inifd();
      do_select();
      check_fd();
    }
}
