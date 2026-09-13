NAME		= push_swap
BIN_DIR		= bin
DEBUG_NAME	= $(BIN_DIR)/push_swap_debug
ANALYSIS_NAME	= $(BIN_DIR)/bfs_analyser
ALL_PATHS_NAME	= $(BIN_DIR)/bfs_all_paths

CC			= cc
CFLAGS		= -Wall -Wextra -Werror
RM			= rm -f

LIBFT_DIR	= ryker_libft
LIBFT		= $(LIBFT_DIR)/libft.a

INCLUDES = -I.

SRCS		= main_push_swap.c \
			  parse_sort_input.c \
			  stack_operation_swap_push.c \
			  stack_operation_rotate.c \
			  stack_operation_reverse_rotate.c \
			  circular_buffer_utils.c \
			  circular_buffer_move_element.c \
			  circular_buffer_insert_remove.c \
			  debug_tools.c \
			  rank_values.c \
			  stack_utils.c \
			  solve.c \
			  bfs.c \
			  bfs_utils.c \
			  bfs_verify_node.c \
			  bfs_operations_swap_push.c \
			  bfs_operations_rotate.c \
			  bfs_operations_reverse_rotate.c \
			  bfs_optimiser_lehmer_rank.c \
			  chunk_extract_optimal.c \
			  DO_NOT_SUBMIT_DEBUG_hidden_bfs.c \

OBJ_DIR		= obj
OBJS		= $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

all: $(NAME)

debug: $(LIBFT)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -DBFS_DEBUG $(INCLUDES) $(SRCS) \
		debug/DO_NOT_SUBMIT_DEBUG_bfs_results.c $(LIBFT) -o $(DEBUG_NAME)

analyse_bfs: $(LIBFT)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) \
		debug/DO_NOT_SUBMIT_DEBUG_bfs_all_permutations.c \
		bfs.c bfs_utils.c bfs_verify_node.c \
		bfs_operations_swap_push.c bfs_operations_rotate.c \
		bfs_operations_reverse_rotate.c bfs_optimiser_lehmer_rank.c \
		circular_buffer_utils.c $(LIBFT) -o $(ANALYSIS_NAME)

analyse_bfs_all_paths: $(LIBFT)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) \
		debug/DO_NOT_SUBMIT_DEBUG_bfs_reverse_all_paths.c \
		bfs_utils.c bfs_verify_node.c bfs_operations_swap_push.c \
		bfs_operations_rotate.c bfs_operations_reverse_rotate.c \
		bfs_optimiser_lehmer_rank.c $(LIBFT) -o $(ALL_PATHS_NAME)

$(LIBFT): FORCE
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

$(OBJ_DIR)/%.o: %.c push_swap.h $(LIBFT_DIR)/ryker_libft.h Makefile
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	$(RM) $(SRCS:.c=.o)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME) $(DEBUG_NAME) $(ANALYSIS_NAME) $(ALL_PATHS_NAME)
	rm -rf $(BIN_DIR)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all debug analyse_bfs analyse_bfs_all_paths clean fclean re

FORCE:

.PHONY: FORCE
