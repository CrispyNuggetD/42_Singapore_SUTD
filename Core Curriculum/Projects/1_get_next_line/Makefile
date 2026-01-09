CC = cc
	#### SEARCH: UN-COMMENT HERE OUT LATER ####
CFLAGS	= -Wall -Wextra -Werror -g #Standard 42 norm
	#### SEARCH: UN-COMMENT HERE OUT LATER ####
#CFLAGS	:= 
AR = ar
AR_FLAGS = rcs

NAME	= libftprintf.a #Library name

SRCS	= get_next_line.c get_next_line_utils.c

	#### SEARCH: COMMENT HERE OUT LATER ####
BONUS	= get_next_line_bonus.c get_next_line_utils_bonus.c
	#### SEARCH: COMMENT HERE OUT LATER ####

OBJS	= $(SRCS:.c=.o)

	#### SEARCH: COMMENT HERE OUT LATER ####
BONUS_OBJ := $(BONUS:.c=.o)
	#### SEARCH: COMMENT HERE OUT LATER ####

all: $(NAME)

$(NAME): $(OBJS)
	@echo "\nArchiving Main Project..." #My echo
	$(AR) $(AR_FLAGS) $(NAME) $(OBJS)
	@echo "\nMain project library archived!\n" #My echo

	#### SEARCH: COMMENT HERE OUT LATER ####
bonus: $(BONUS_OBJ)
	@echo "\nArchiving Bonus Project..." #My echo
	rm -f $(NAME)
	$(AR) $(AR_FLAGS) $(NAME) $(BONUS_OBJ)
	@echo "\nBonus project library archived!\n" #My echo
	#### SEARCH: COMMENT HERE OUT LATER ####

%.o: %.c ft_printf.h
	@echo "\nCompiling Project's .c file..." #My echo
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "\nProject's .c file compiled!" #My echo
	
clean:
	@echo "\nCleaning project object files..." #My echo
	rm -f $(OBJS)
	rm -f $(BONUS_OBJ)
	$(MAKE) -C $(LIBFT_DIR) clean
	@echo "\nProject objects cleaned!" #My echo

fclean: clean
	@echo "\nFull cleaning project library archive..." #My echo
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean
	@echo "Project library archive cleaned!" #My echo

re: fclean all 
	@echo "\nProject library re-built!" #My echo

#	#### SEARCH: UN-COMMENT HERE OUT LATER ####
#.PHONY: all clean fclean re
#	#### SEARCH: UN-COMMENT HERE OUT LATER ####

	#### SEARCH: COMMENT HERE OUT LATER ####
.PHONY: all clean fclean re bonus
	#### SEARCH: COMMENT HERE OUT LATER ####


###########################################
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
