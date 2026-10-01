NAME		= push_swap
BONUS_NAME	= checker
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
HEADERS = $(wildcard $(INC_DIR)/*.h) $(LIBFT_HEADERS)

CBUF_DIR = data_structures/circular_buffer
BFS_DIR = algorithms/sorting_algorithms/Brute_force/BFS
PARSING_DIR = push_swap_requirements/parsing_input
OPERATIONS_DIR = push_swap_requirements/stack_operations

SRCS		= main_push_swap.c \
			  algorithms/algorithm_config.c \
			  $(PARSING_DIR)/parse_sort_input.c \
			  $(PARSING_DIR)/input_storage.c \
			  $(OPERATIONS_DIR)/stack_operation_swap_push.c \
			  $(OPERATIONS_DIR)/stack_operation_rotate.c \
			  $(OPERATIONS_DIR)/stack_operation_reverse_rotate.c \
			  $(CBUF_DIR)/circular_buffer_utils.c \
			  $(CBUF_DIR)/circular_buffer_storage.c \
			  $(CBUF_DIR)/circular_buffer_alignment.c \
			  $(CBUF_DIR)/circular_buffer_get_info.c \
			  $(CBUF_DIR)/circular_buffer_move_element.c \
			  $(CBUF_DIR)/circular_buffer_insert_remove.c \
			  printers/debug_tools.c \
			  printers/debug_solutions.c \
			  printers/debug_lookahead.c \
			  printers/debug_search_progress.c \
			  printers/debug_search_pruned.c \
			  printers/debug_pass_progress.c \
			  printers/debug_search_result.c \
			  printers/debug_status_bar.c \
			  printers/debug_bfs.c \
			  printers/debug_bfs_status.c \
			  printers/debug_chunks.c \
			  printers/solution_printers.c \
			  algorithms/rank_values.c \
			  solution_utils.c \
			  solve.c \
			  algorithms/sorting_algorithms/Exact_hardcoded/hardcode.c \
			  algorithms/sorting_algorithms/Exact_hardcoded/precomputed_ranks_bfs.c \
			  $(BFS_DIR)/bfs_main.c \
			  $(BFS_DIR)/bfs_search.c \
			  $(BFS_DIR)/bfs_state_representation.c \
			  $(BFS_DIR)/bfs_utils.c \
			  $(BFS_DIR)/bfs_array_helpers.c \
			  $(BFS_DIR)/bfs_verify_node.c \
			  $(BFS_DIR)/push_swap_operations_bfs/bfs_operations_swap_push.c \
			  $(BFS_DIR)/push_swap_operations_bfs/bfs_operations_rotate.c \
			  $(BFS_DIR)/push_swap_operations_bfs/bfs_operations_reverse_rotate.c \
			  $(BFS_DIR)/bfs_optimiser_lehmer_rank.c \
			  algorithms/sorting_algorithms/Cost_aware/Greedy/greedy_reinsertion.c \
			  algorithms/sorting_algorithms/Cost_aware/Greedy/greedy_lookahead_utils.c \
			  algorithms/sorting_algorithms/Cost_aware/Greedy/greedy_choose_plan.c \
			  algorithms/sorting_algorithms/Cost_aware/Greedy/greedy_branch.c \
			  algorithms/sorting_algorithms/Cost_aware/Greedy/greedy_stages.c \
			  algorithms/sorting_algorithms/Cost_aware/Greedy/greedy_prepare.c \
			  algorithms/sorting_algorithms/Cost_aware/Greedy/greedy_execute.c

OBJ_DIR		= obj
OBJS		= $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

# Checker reuses parsing, stacks and operations without linking the solver main.
BONUS_SRCS = bonus/main_checker_bonus.c \
	bonus/checker_util_bonus.c \
	$(PARSING_DIR)/parse_sort_input.c \
	$(PARSING_DIR)/input_storage.c \
	algorithms/rank_values.c \
	$(OPERATIONS_DIR)/stack_operation_swap_push.c \
	$(OPERATIONS_DIR)/stack_operation_rotate.c \
	$(OPERATIONS_DIR)/stack_operation_reverse_rotate.c \
	$(CBUF_DIR)/circular_buffer_utils.c \
	$(CBUF_DIR)/circular_buffer_storage.c \
	$(CBUF_DIR)/circular_buffer_move_element.c \
	$(CBUF_DIR)/circular_buffer_insert_remove.c \
	solution_utils.c
BONUS_OBJS = $(addprefix $(OBJ_DIR)/,$(BONUS_SRCS:.c=.o))

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(LIBFT) $(BONUS_OBJS)
	$(CC) $(CFLAGS) $(BONUS_OBJS) $(LIBFT) -o $@

$(OBJ_DIR)/bonus/%.o: $(INC_DIR)/checker_bonus.h

all: $(NAME)

debug: $(DEBUG_NAME)

$(DEBUG_NAME): $(addprefix $(SRC_DIR)/,$(SRCS)) $(HEADERS) \
	debug/DO_NOT_SUBMIT_DEBUG_bfs_results.c \
	debug/DO_NOT_SUBMIT_DEBUG_bfs_results.h $(LIBFT) Makefile
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -DBFS_DEBUG $(INCLUDES) $(addprefix $(SRC_DIR)/,$(SRCS)) \
		debug/DO_NOT_SUBMIT_DEBUG_bfs_results.c $(LIBFT) -o $(DEBUG_NAME)

BFS_SRCS = bfs_utils.c bfs_array_helpers.c bfs_verify_node.c \
	push_swap_operations_bfs/bfs_operations_swap_push.c \
	push_swap_operations_bfs/bfs_operations_rotate.c \
	push_swap_operations_bfs/bfs_operations_reverse_rotate.c \
	bfs_optimiser_lehmer_rank.c
ANALYSIS_SRCS = debug/DO_NOT_SUBMIT_DEBUG_bfs_all_permutations.c \
	$(SRC_DIR)/algorithms/algorithm_config.c \
	$(SRC_DIR)/printers/debug_tools.c \
	$(SRC_DIR)/printers/debug_bfs.c \
	$(SRC_DIR)/printers/debug_bfs_status.c \
	$(SRC_DIR)/$(BFS_DIR)/bfs_main.c \
	$(SRC_DIR)/$(BFS_DIR)/bfs_search.c \
	$(SRC_DIR)/$(BFS_DIR)/bfs_state_representation.c \
	$(SRC_DIR)/$(CBUF_DIR)/circular_buffer_utils.c \
	$(SRC_DIR)/$(CBUF_DIR)/circular_buffer_storage.c \
	$(addprefix $(SRC_DIR)/$(BFS_DIR)/,$(BFS_SRCS))
ALL_PATHS_SRCS = debug/DO_NOT_SUBMIT_DEBUG_bfs_reverse_all_paths.c \
	$(addprefix $(SRC_DIR)/$(BFS_DIR)/,$(BFS_SRCS))

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
	$(RM) $(NAME) $(BONUS_NAME) $(DEBUG_NAME) $(ANALYSIS_NAME) $(ALL_PATHS_NAME)
	$(RM) $(BIN_DIR)/generator
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all bonus generator debug analyse_bfs analyse_bfs_all_paths clean fclean re

FORCE:

.PHONY: FORCE
