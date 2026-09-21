NAME := fdf
BONUS_MARKER := .bonus
CC := cc
CFLAGS := -Wall -Wextra -Werror

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := includes
LIBFT_DIR := libft
UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
	MLX_REPO := https://github.com/dannywillems/minilibx-mac-osx.git
	MLX_REV := bae208e5502e200baf4b9e8188b7751e72ce5590
	MLX_DIR := .deps/minilibx-macos-$(MLX_REV)
	MLX_FLAGS := -L$(MLX_DIR) -lmlx -framework OpenGL -framework AppKit
else
	MLX_REPO := https://github.com/42Paris/minilibx-linux.git
	MLX_REV := f07d00f07c5c652223f505b526f84dab73cf2598
	MLX_DIR := .deps/minilibx-linux-$(MLX_REV)
	MLX_FLAGS := -L$(MLX_DIR) -lmlx -lXext -lX11 -lm
endif

COMMON_SRCS := mlx_init.c events.c pixel.c map_dimensions.c map_load.c \
	projection.c projection_bounds.c line.c colour.c
SRCS := main.c $(COMMON_SRCS)
BONUS_SRCS := main_bonus.c rotation_events_bonus.c game_bonus.c \
	game_render_bonus.c game_window_bonus.c perspective_bonus.c $(COMMON_SRCS)
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)
BONUS_OBJS := $(BONUS_SRCS:%.c=$(OBJ_DIR)/%.o)
LIBFT := $(LIBFT_DIR)/libft.a
MLX := $(MLX_DIR)/libmlx.a
INCLUDES = -I$(INC_DIR) -I$(MLX_DIR)

all: $(NAME)

bonus: $(BONUS_MARKER)

ifneq ($(wildcard $(BONUS_MARKER)),)
$(NAME): FORCE
endif

FORCE:

$(BONUS_MARKER): $(BONUS_OBJS) $(LIBFT) $(MLX)
	$(CC) $(CFLAGS) $(BONUS_OBJS) $(LIBFT) \
		$(MLX_FLAGS) -o $(NAME)
	touch $(BONUS_MARKER)

$(NAME): $(OBJS) $(LIBFT) $(MLX)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(MLX_FLAGS) -o $(NAME)
	rm -f $(BONUS_MARKER)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/fdf.h $(LIBFT_DIR)/ryker_libft.h Makefile | $(MLX_DIR)/.ready
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/projection.o $(OBJ_DIR)/projection_bounds.o: \
	$(INC_DIR)/projection.h

$(OBJ_DIR)/main_bonus.o $(OBJ_DIR)/rotation_events_bonus.o \
$(OBJ_DIR)/game_bonus.o $(OBJ_DIR)/game_render_bonus.o \
$(OBJ_DIR)/game_window_bonus.o $(OBJ_DIR)/perspective_bonus.o: \
	$(INC_DIR)/rotation_bonus.h

$(LIBFT): FORCE
	$(MAKE) -C $(LIBFT_DIR)

deps: $(MLX_DIR)/.ready

$(MLX_DIR)/.ready:
	@mkdir -p .deps
	@set -eu; \
		tmp=$$(mktemp -d "$(MLX_DIR).tmp.XXXXXX"); \
		trap 'rm -rf "$$tmp"' EXIT HUP INT TERM; \
		git -C "$$tmp" init -q; \
		git -C "$$tmp" fetch -q --depth 1 "$(MLX_REPO)" "$(MLX_REV)"; \
		git -C "$$tmp" checkout -q --detach FETCH_HEAD; \
		test -f "$$tmp/mlx.h"; \
		touch "$$tmp/.ready"; \
		mv "$$tmp" "$(MLX_DIR)"

$(MLX): $(MLX_DIR)/.ready
	$(MAKE) -C $(MLX_DIR)

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(BONUS_MARKER)
	$(MAKE) -C $(LIBFT_DIR) clean
	@if test -f "$(MLX_DIR)/Makefile"; then \
		$(MAKE) -C $(MLX_DIR) clean; \
	fi

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all bonus clean fclean re FORCE
