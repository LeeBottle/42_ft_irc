#include "bircd.hpp"

void	fd::clean_fd()
{
	type = 0;
	fct_read = NULL;
	fct_write = NULL;
	buf_read.clear();
	buf_write.clear();
	nickname.clear();
	username.clear();
	close_after_write = false;
}
