#include "bircd.hpp"

void	env::get_opt(int ac, char **av, env &e)
{
	if (ac != 3)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		exit(1);
	}
	std::stringstream ss(av[1]);
	if (!(ss >> port) || !ss.eof())
	{
		std::cerr << "Error: Invalid port number" << std::endl;
		exit(1);
	}
	e.password = av[2];
}

