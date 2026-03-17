NAME = irc
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP -I server/include
OBJ_DIR = obj
SRC_ALL = main.cpp server/src/Server.cpp server/src/Channel.cpp server/src/Socket.cpp server/src/Client.cpp
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
	@printf "$(GREEN) $(CC) $(CFLAGS) -o $@ -c $< $(NC)\n"
	@$(CC) $(CFLAGS) -o $@ -c $<

-include $(DEPS)

$(NAME): $(OBJ)
	@printf "$(GREEN) $(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(NC)\n"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ)
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
