NAME		= push_swap
DEBUG_NAME	= push_swap_debug
ANALYSIS_NAME	= bfs_analyser
ALL_PATHS_NAME	= bfs_all_paths

CC			= cc
CFLAGS		= -Wall -Wextra -Werror
RM			= rm -f

LIBFT_DIR	= libft/0_libft
LIBFT		= $(LIBFT_DIR)/libft.a

INCLUDES	= -I. -I$(LIBFT_DIR)

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
			  DO_NOT_SUBMIT_DEBUG_hidden_bfs.c \

OBJS		= $(SRCS:.c=.o)

all: $(NAME)

debug: $(LIBFT)
	$(CC) $(CFLAGS) -DBFS_DEBUG $(INCLUDES) $(SRCS) \
		debug/DO_NOT_SUBMIT_DEBUG_bfs_results.c $(LIBFT) -o $(DEBUG_NAME)

analyse_bfs:
	$(CC) $(CFLAGS) $(INCLUDES) \
		debug/DO_NOT_SUBMIT_DEBUG_bfs_all_permutations.c \
		bfs.c bfs_utils.c bfs_verify_node.c \
		bfs_operations_swap_push.c bfs_operations_rotate.c \
		bfs_operations_reverse_rotate.c bfs_optimiser_lehmer_rank.c \
		circular_buffer_utils.c -o $(ANALYSIS_NAME)

analyse_bfs_all_paths:
	$(CC) $(CFLAGS) $(INCLUDES) \
		debug/DO_NOT_SUBMIT_DEBUG_bfs_reverse_all_paths.c \
		bfs_utils.c bfs_verify_node.c bfs_operations_swap_push.c \
		bfs_operations_rotate.c bfs_operations_reverse_rotate.c \
		bfs_optimiser_lehmer_rank.c -o $(ALL_PATHS_NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

%.o: %.c push_swap.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME) $(DEBUG_NAME) $(ANALYSIS_NAME) $(ALL_PATHS_NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all debug analyse_bfs analyse_bfs_all_paths clean fclean re
