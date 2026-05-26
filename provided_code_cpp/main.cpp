#include "bircd.hpp"

int	main(int ac, char **av)
{
  env	e;

  e.init_env();
  e.get_opt(ac, av, e);
  e.srv_create();
  e.main_loop();
  return (0);
}
