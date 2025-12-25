CC	= cc
CFLAGS	= -Wall -Wextra -Werror #Standard 42 norm
AR_FLAGS = rcs
AR = ar

NAME	= libft.a #Library name

SRCS	= ft_isalpha.c ft_isdigit.c ft_isalnum.c ft_isascii.c ft_isprint.c ft_strlen.c ft_memset.c ft_bzero.c ft_memcpy.c ft_memmove.c ft_strlcpy.c ft_strlcat.c ft_toupper.c ft_tolower.c ft_strchr.c ft_strrchr.c ft_strncmp.c ft_memchr.c ft_memcmp.c ft_strnstr.c ft_atoi.c ft_calloc.c ft_strdup.c ft_substr.c ft_strjoin.c ft_strtrim.c ft_split.c ft_itoa.c ft_strmapi.c ft_striteri.c ft_putchar_fd.c ft_putstr_fd.c ft_putendl_fd.c ft_putnbr_fd.c ft_lstnew.c ft_lstadd_front.c ft_lstsize.c ft_lstlast.c ft_lstadd_back.c ft_lstdelone.c ft_lstclear.c ft_lstiter.c ft_lstmap.c

	#### SEARCH: COMMENT HERE OUT LATER ####
#BONUS := ft_lstnew.c ft_lstadd_front.c ft_lstsize.c ft_lstlast.c ft_lstadd_back.c ft_lstdelone.c ft_lstclear.c ft_lstiter.c ft_lstmap.c
	#### SEARCH: COMMENT HERE OUT LATER ####

OBJS	= $(SRCS:.c=.o)
	#### SEARCH: COMMENT HERE OUT LATER ####
#BONUS_OBJ := $(BONUS:.c=.o)
	#### SEARCH: COMMENT HERE OUT LATER ####

all: $(NAME)

$(NAME): $(OBJS) 
	@echo "\nArchiving ft_libft Project...\n" #My echo
	$(AR) $(AR_FLAGS) $(NAME) $(OBJS)
	@echo "\nLibft library archived!\n" #My echo
	
	#### SEARCH: COMMENT HERE OUT LATER ####
#bonus: $@ $(BONUS_OBJ)
#	$(AR) $(AR_FLAGS) $(NAME) $(BONUS_OBJ)
	#### SEARCH: COMMENT HERE OUT LATER ####

%.o: %.c libft.h
	@echo "\nCompiling ft_libft Project's .c file..." #My echo
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "\nft_libft Project's .c file compiled!" #My echo
	
clean:
	@echo "\nCleaning ft_libft object files...\n" #My echo
	rm -f $(OBJS)
	@echo "\nft_libft Objects cleaned!\n" #My echo


fclean: clean
	@echo "\nFull cleaning ft_libft library archive...\n" #My echo
	rm -f $(NAME)
	@echo "\nft_libft Library archive cleaned!" #My echo


re: fclean all
	@echo "\nft_libft Library Re-built!\n" #My echo


	#### SEARCH: UN-COMMENT HERE OUT LATER ####
.PHONY: all clean fclean re
	#tells make these targets aren’t real files.
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
	
###########################################
###########################################
