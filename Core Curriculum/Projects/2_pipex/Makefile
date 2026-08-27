NAME := pipex
CC := cc
CFLAGS := -Wall -Wextra -Werror

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := includes
LIBFT_DIR := libft/0_libft

SRCS := main.c
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)
LIBFT := $(LIBFT_DIR)/libft.a
INCLUDES := -I$(INC_DIR) -I$(LIBFT_DIR)

all: $(NAME)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/pipex.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
