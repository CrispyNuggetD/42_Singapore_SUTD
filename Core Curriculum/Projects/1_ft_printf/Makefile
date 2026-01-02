CC	= cc
CFLAGS	= -Wall -Wextra -Werror #Standard 42 norm
AR = ar
AR_FLAGS = rcs

INCS	= -I. -Ilibft #Where to look for header files

NAME	= libftprintf.a #Library name

SRCS	= ft_printf.c ft_parse_spec.c ft_print_d_i.c #ft_print_c.c ft_print_s.c ft_print_nbr.c \
		#ft_print_hex.c ft_print_ptr.c

	#### SEARCH: COMMENT HERE OUT LATER ####
#BONUS := ft_lstnew.c ft_lstadd_front.c 
	#### SEARCH: COMMENT HERE OUT LATER ####

OBJS	= $(SRCS:.c=.o)
	#### SEARCH: COMMENT HERE OUT LATER ####
#BONUS_OBJ := $(BONUS:.c=.o)
	#### SEARCH: COMMENT HERE OUT LATER ####

LIBFT_DIR	= libft
LIBFT		= $(LIBFT_DIR)/libft.a
# NEW to Milestone 1 (Linking Libft)

all: $(NAME)

# 1) I Build libft first
$(LIBFT):
	@echo "\nArchiving libft first..." #My echo
	$(MAKE) -C $(LIBFT_DIR)
	@echo "\nLibft library archived!" #My echo
	
# 2) Then I build the combined archive at ft_printf root
$(NAME): $(LIBFT) $(OBJS)
	@echo "\nMoving Libft library to be compiled!" #My echo
	cp $(LIBFT) $(NAME)
	@echo "\nShifted Libft library!\n" #My echo
	@echo "\nArchiving Main Project..." #My echo
	$(AR) $(AR_FLAGS) $(NAME) $(OBJS)
	@echo "\nMain project library archived!\n" #My echo

	#### SEARCH: COMMENT HERE OUT LATER ####
#bonus: $@ $(BONUS_OBJ)
#	$(AR) $(AR_FLAGS) $(NAME) $(BONUS_OBJ)
	#### SEARCH: COMMENT HERE OUT LATER ####

%.o: %.c ft_printf.h
	@echo "\nCompiling Project's .c file..." #My echo
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "\nProject's .c file compiled!" #My echo
	
clean:
	@echo "\nCleaning project object files..." #My echo
	rm -f $(OBJS)
	$(MAKE) -C $(LIBFT_DIR) clean
	@echo "\nProject objects cleaned!" #My echo

fclean: clean
	@echo "\nFull cleaning project library archive..." #My echo
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean
	@echo "Project library archive cleaned!" #My echo

re: fclean all 
	@echo "\nProject library re-built!" #My echo

	#### SEARCH: UN-COMMENT HERE OUT LATER ####
.PHONY: all clean fclean re
	#### SEARCH: UN-COMMENT HERE OUT LATER ####

	#### SEARCH: COMMENT HERE OUT LATER ####
#.PHONY: all clean fclean re bonus
	#### SEARCH: COMMENT HERE OUT LATER ####


###########################################
###########################################

# NEW to Milestone 1 (Linking Libft)

# For: INCS	= -I. -Ilibft
# -I is a compiler flag (for cc, gcc, clang) meaning:
# Add this directory to the include search path.
# So:
# -I. --> The dot does a “Also look in the current directory”
# -Ilibft --> “Also look in the libft/ directory”

# cp copies it to ./libftprintf.a

###########################################

# To build libft.a, you must **first** have all the .o files. 
# If any .o is newer than libft.a (or libft.a is missing)
# THEN run the recipe that follows.
# Then, build libft/libft.a via $(MAKE) -C libft

###########################################

# ar = archiver; it bundles .o files into a .a static library.
# r = replace or add files to archive
# c = create the archive if it doesn’t exists
# s = build index (symbol table) → makes linking faster

# $< = first dependency (the .c)
# $@ = target (the .o)

###########################################

# clean (clean workspace (intermediate stuff)), delete .o files
#fclean (full clean), delete .o + final binary/lib
#all (build all main targets), build the main output (libft.a)
#re (rebuild), full clean + rebuild everything

# .PHONY: #tells make these targets aren’t real files.
	
###########################################
###########################################
