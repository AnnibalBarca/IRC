NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP -I server/include -I client/include
OBJ_DIR = obj
SRC_ALL = main.cpp server/src/Server.cpp server/src/Channel.cpp server/src/Socket.cpp server/src/Nick.cpp server/src/ErrorReplies.cpp server/src/Mode.cpp server/src/Pass.cpp server/src/Kick.cpp server/src/Topic.cpp server/src/Invite.cpp server/src/User.cpp server/src/Client.cpp server/src/Join.cpp server/src/Quit.cpp
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_ALL:.cpp=.o))
DEPS = $(OBJ:.o=.d)

GREEN=\033[0;32m
BLUE=\033[38;2;64;224;208m
RED=\033[0;91m
WHITE=\033[2;37m
NC=\033[0m

all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR)/%.o: %.cpp Makefile
	@printf "$(WHITE) mkdir -p $(dir $@) $(NC)\n"
	@mkdir -p $(dir $@)
	@printf "$(GREEN) $(CXX) $(CXXFLAGS) -o $@ -c $< $(NC)\n"
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

-include $(DEPS)

$(NAME): $(OBJ)
	@printf "$(GREEN) $(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ) $(NC)\n"
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)
	@printf "$(BLUE)✓ COMPILATION SUCCESS! ✓$(NC)\n"

$(OBJ_DIR):
	@printf "$(WHITE) mkdir -p $(OBJ_DIR) $(NC)\n"
	@mkdir -p $(OBJ_DIR)

clean:
	@printf "$(RED) rm -rf $(OBJ_DIR) $(NC)\n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf "$(RED) rm -f $(NAME) $(NC)\n"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
