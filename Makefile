NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98
INCLUDES = -I includes

SRC_DIR = srcs
OBJ_DIR = obj

SRCS = main.cpp	\
	   Server.cpp	\
	   Client.cpp
	   
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
