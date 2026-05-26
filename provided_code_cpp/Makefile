
SRCS =	main.cpp init_env.cpp clean_fd.cpp get_opt.cpp main_loop.cpp \
	do_select.cpp check_fd.cpp \
	srv_create.cpp srv_accept.cpp \
	client_read.cpp client_write.cpp \
	init_epoll.cpp check_epoll.cpp

OBJS = ${SRCS:.cpp=.o}

NAME = ircserv

CFLAGS = -I. -g -std=c++98

LDFLAGS = 

CC = c++ 
RM = rm -f

${NAME}:	${OBJS}
		${CC} -o ${NAME} ${OBJS} ${LDFLAGS}

all:		${NAME}

clean:
		${RM} ${OBJS} *~ #*#

fclean:		clean
		${RM} ${NAME}

re:		fclean all
