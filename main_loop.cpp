#include "bircd.hpp"

void	env::main_loop()
{
	while (1)
	{
		do_epoll();
		check_epoll();
	}
}
