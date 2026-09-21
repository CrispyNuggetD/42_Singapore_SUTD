NAME		= push_swap
BIN_DIR		= bin
DEBUG_NAME	= $(BIN_DIR)/push_swap_debug
ANALYSIS_NAME	= $(BIN_DIR)/bfs_analyser
ALL_PATHS_NAME	= $(BIN_DIR)/bfs_all_paths

CC			= cc
CFLAGS		= -Wall -Wextra -Werror
RM			= rm -f

LIBFT_DIR	= libft
LIBFT		= $(LIBFT_DIR)/libft.a

SRC_DIR		= src
INC_DIR		= includes
INCLUDES	= -I$(INC_DIR) -Idebug
LIBFT_HEADERS = $(LIBFT_DIR)/ryker_libft.h \
	$(wildcard $(LIBFT_DIR)/0_libft/*.h $(LIBFT_DIR)/1_ft_printf/*.h \
	$(LIBFT_DIR)/1_get_next_line/*.h $(LIBFT_DIR)/ryker_ft/*.h \
	$(LIBFT_DIR)/ryker_ft/*/*.h)
HEADERS = $(INC_DIR)/push_swap.h $(LIBFT_HEADERS)

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
			  DO_NOT_SUBMIT_DEBUG_hidden_bfs.c

OBJ_DIR		= obj
OBJS		= $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

all: $(NAME)

debug: $(DEBUG_NAME)

$(DEBUG_NAME): $(addprefix $(SRC_DIR)/,$(SRCS)) $(HEADERS) \
	debug/DO_NOT_SUBMIT_DEBUG_bfs_results.c \
	debug/DO_NOT_SUBMIT_DEBUG_bfs_results.h $(LIBFT) Makefile
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -DBFS_DEBUG $(INCLUDES) $(addprefix $(SRC_DIR)/,$(SRCS)) \
		debug/DO_NOT_SUBMIT_DEBUG_bfs_results.c $(LIBFT) -o $(DEBUG_NAME)

BFS_SRCS = bfs_utils.c bfs_verify_node.c bfs_operations_swap_push.c \
	bfs_operations_rotate.c bfs_operations_reverse_rotate.c \
	bfs_optimiser_lehmer_rank.c
ANALYSIS_SRCS = debug/DO_NOT_SUBMIT_DEBUG_bfs_all_permutations.c \
	$(addprefix $(SRC_DIR)/,bfs.c circular_buffer_utils.c $(BFS_SRCS))
ALL_PATHS_SRCS = debug/DO_NOT_SUBMIT_DEBUG_bfs_reverse_all_paths.c \
	$(addprefix $(SRC_DIR)/,$(BFS_SRCS))

analyse_bfs: $(ANALYSIS_NAME)

$(ANALYSIS_NAME): $(ANALYSIS_SRCS) $(HEADERS) $(LIBFT) Makefile
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(ANALYSIS_SRCS) $(LIBFT) -o $@

analyse_bfs_all_paths: $(ALL_PATHS_NAME)

$(ALL_PATHS_NAME): $(ALL_PATHS_SRCS) $(HEADERS) $(LIBFT) Makefile
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(ALL_PATHS_SRCS) $(LIBFT) -o $@

generator: $(BIN_DIR)/generator

$(BIN_DIR)/generator: debug/DO_NOT_SUBMIT_DEBUG_generator.c Makefile
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(LIBFT): FORCE
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) Makefile
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME) $(DEBUG_NAME) $(ANALYSIS_NAME) $(ALL_PATHS_NAME)
	$(RM) $(BIN_DIR)/generator
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all generator debug analyse_bfs analyse_bfs_all_paths clean fclean re

FORCE:

.PHONY: FORCE
