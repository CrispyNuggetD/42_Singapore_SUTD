NAME := fdf
CC := cc
CFLAGS := -Wall -Wextra -Werror

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := includes
LIBFT_DIR := libft/0_libft
GNL_DIR := libft/1_get_next_line
UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
	MLX_DIR := minilibx-macos
	MLX_FLAGS := -L$(MLX_DIR) -lmlx -framework OpenGL -framework AppKit
else
	MLX_DIR := minilibx-linux
	MLX_FLAGS := -L$(MLX_DIR) -lmlx -lXext -lX11 -lm
endif

SRCS := main.c mlx_init.c events.c pixel.c map_dimensions.c map_load.c \
	projection.c projection_bounds.c line.c colour.c
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)
GNL_SRCS := get_next_line.c get_next_line_utils.c
GNL_OBJS := $(GNL_SRCS:%.c=$(OBJ_DIR)/gnl_%.o)
LIBFT := $(LIBFT_DIR)/libft.a
MLX := $(MLX_DIR)/libmlx.a
INCLUDES := -I$(INC_DIR) -I$(LIBFT_DIR) -I$(GNL_DIR) -I$(MLX_DIR)

all: $(NAME)

$(NAME): $(OBJS) $(GNL_OBJS) $(LIBFT) $(MLX)
	$(CC) $(CFLAGS) $(OBJS) $(GNL_OBJS) $(LIBFT) $(MLX_FLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/fdf.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/projection.o $(OBJ_DIR)/projection_bounds.o: \
	$(INC_DIR)/projection.h

$(OBJ_DIR)/gnl_%.o: $(GNL_DIR)/%.c $(GNL_DIR)/get_next_line.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(MLX):
	$(MAKE) -C $(MLX_DIR)

clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean
	$(MAKE) -C $(MLX_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
