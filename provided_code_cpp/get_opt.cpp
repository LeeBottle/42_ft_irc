#include "bircd.hpp"

void	env::get_opt(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " port" << std::endl;
		exit(1);
	}
  port = atoi(av[1]);
}

