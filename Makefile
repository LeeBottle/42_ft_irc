NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I includes

SRC_DIR = srcs
OBJ_DIR = obj

SRCS = main.cpp \
       channel/Channel.cpp \
       channel/ChannelManager.cpp \
       channel/ChannelInviteList.cpp \
       channel/ChannelMemberList.cpp \
       channel/ChannelModeState.cpp \
       channel/ChannelOperatorList.cpp \
       client/Client.cpp \
       client/ClientBuffer.cpp \
       client/ClientManager.cpp \
       client/ClientRequestHandler.cpp \
       command/Command.cpp \
       command/CommandParser.cpp \
       command/CommandRouter.cpp \
       command/CommandHandlers.cpp \
       command/CommandChannelBase.cpp \
       command/CommandChannelJoin.cpp \
       command/CommandChannelMessage.cpp \
       command/CommandChannelInfo.cpp \
       command/CommandChannelOperator.cpp \
       command/CommandChannelMode.cpp \
       command/CommandUserHandler.cpp \
       server/Server.cpp \
       server/ServerSocket.cpp

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
