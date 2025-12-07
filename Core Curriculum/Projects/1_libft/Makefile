CC	= cc
CFLAGS	= -Wall -Wextra -Werror #Standard 42 norm

NAME	= libft.a #Library name

SRCS	= ft_isalpha.c ft_strlen.c ft_itoa.c
OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS) 
# To build libft.a, you must **first** have all the .o files. If any .o is newer than libft.a (or libft.a is missing), THEN run the recipe that follows.
# ar = archiver; it bundles .o files into a .a static library.
# r = replace or add files to archive
# c = create the archive if it doesn’t exists
# s = build index (symbol table) → makes linking faster
	@echo "\nArchiving..." #My echo
	ar rcs $(NAME) $(OBJS)

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

.PHONY: all clean fclean re
#tells make these targets aren’t real files.
