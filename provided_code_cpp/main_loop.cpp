#include "bircd.hpp"

void	env::main_loop()
{
	while (1)
	{
		do_select();
		check_epoll();
	}
}
