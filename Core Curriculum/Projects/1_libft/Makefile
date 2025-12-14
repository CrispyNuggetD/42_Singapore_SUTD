CC	= cc
CFLAGS	= -Wall -Wextra -Werror #Standard 42 norm
AR_FLAGS = rcs
AR = ar

NAME	= libft.a #Library name

SRCS	= ft_isalpha.c ft_isdigit.c ft_isalnum.c ft_isascii.c ft_isprint.c ft_strlen.c ft_memset.c ft_bzero.c ft_memcpy.c ft_memmove.c ft_strlcpy.c ft_strlcat.c ft_toupper.c ft_tolower.c ft_strchr.c ft_strrchr.c ft_strncmp.c ft_memchr.c ft_memcmp.c ft_strnstr.c ft_atoi.c ft_calloc.c ft_strdup.c ft_substr.c ft_strjoin.c ft_strtrim.c ft_split.c ft_itoa.c ft_strmapi.c ft_striteri.c ft_putchar_fd.c ft_putstr_fd.c ft_putendl_fd.c ft_putnbr_fd.c 

#### SEARCH: COMMENT HERE OUT LATER ####
BONUS := ft_lstnew.c ft_lstadd_front.c ft_lstsize.c ft_lstlast.c ft_lstadd_back.c
#### SEARCH: COMMENT HERE OUT LATER ####

OBJS	= $(SRCS:.c=.o)
#### SEARCH: COMMENT HERE OUT LATER ####
BONUS_OBJ := $(BONUS:.c=.o)
#### SEARCH: COMMENT HERE OUT LATER ####

all: $(NAME)

$(NAME): $(OBJS) 
# To build libft.a, you must **first** have all the .o files. If any .o is newer than libft.a (or libft.a is missing), THEN run the recipe that follows.
# ar = archiver; it bundles .o files into a .a static library.
# r = replace or add files to archive
# c = create the archive if it doesn’t exists
# s = build index (symbol table) → makes linking faster
	@echo "\nArchiving..." #My echo
	$(AR) $(AR_FLAGS) $(NAME) $(OBJS)

#### SEARCH: COMMENT HERE OUT LATER ####
bonus: $@ $(BONUS_OBJ)
	$(AR) $(AR_FLAGS) $(NAME) $(BONUS_OBJ)
#### SEARCH: COMMENT HERE OUT LATER ####

# $< = first dependency (the .c)
# $@ = target (the .o)
%.o: %.c libft.h
	@echo "\nMaking..." #My echo
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	@echo "\nCleaning..." #My echo
	rm -f $(OBJS)
	@echo "Objects cleaned!" #My echo
	@# clean (clean workspace (intermediate stuff)), delete .o files

fclean: clean
	@echo "\nFull cleaning..." #My echo
	rm -f $(NAME)
	@echo "Objects fully cleaned!" #My echo
	@#fclean (full clean), delete .o + final binary/lib

re: fclean all
	@echo "\nLibrary rebuilt!" #My echo
	@#re (rebuild), full clean + rebuild everything
	@#all (build all main targets), build the main output (libft.a)

#### SEARCH: UNCOMMENT HERE OUT LATER ####
#.PHONY: all clean fclean re

#### SEARCH: COMMENT HERE OUT LATER ####
.PHONY: all clean fclean re bonus
#### SEARCH: COMMENT HERE OUT LATER ####

#tells make these targets aren’t real files.
