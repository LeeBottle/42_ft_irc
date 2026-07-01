NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I includes
DEPFLAGS = -MMD -MP

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
       client/ClientPollEventHandler.cpp  \
       client/ClientSocketReceiver.cpp    \
       client/ClientSocketSender.cpp      \
       parser/Parser.cpp \
       command/CommandBase.cpp \
       command/CommandJoinPart.cpp \
       command/CommandMessage.cpp \
       command/CommandInfo.cpp \
       command/CommandInviteKick.cpp \
       command/CommandMode.cpp \
       command/CommandModeEdit.cpp \
       command/CommandModePrepare.cpp \
       command/CommandModeApply.cpp \
       command/CommandModeParameter.cpp \
       command/CommandModeOperator.cpp \
       command/CommandUser.cpp \
       server/Server.cpp \
       server/ServerMessageSwitch.cpp \
       server/ServerSocket.cpp

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
