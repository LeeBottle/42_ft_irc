SRCS =	main.cpp init_env.cpp clean_fd.cpp get_opt.cpp main_loop.cpp \
	do_epoll.cpp \
	srv_create.cpp srv_accept.cpp \
	client_read.cpp client_read_handle.cpp client_read_broadcast.cpp\
	client_write.cpp \
	init_epoll.cpp check_epoll.cpp

OBJS = ${SRCS:.cpp=.o}

NAME = ircserv

CXXFLAGS = -I. -g -Wall -Wextra -Werror -std=c++98

LDFLAGS = 

CXX = c++ 
RM = rm -f

${NAME}:	${OBJS}
	${CXX} ${CXXFLAGS} -o ${NAME} ${OBJS} ${LDFLAGS}

%.o: %.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@

all:		${NAME}

clean:
		${RM} ${OBJS} *~ #*#

fclean:		clean
		${RM} ${NAME}

re:		fclean all
