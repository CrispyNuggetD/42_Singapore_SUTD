Add to home as Hidden File a file using "vim .vimrc" containing:

1. set number, is to have line numbers.

2. For automatic multiline comments with a star prefix in Vim

	+=: This modifier means "append to the option's current value". This is useful for adding new behaviors without overwriting any that are already set by default or by filetype plugins.
ro: The value being added, which consists of two flag characters:
	
	r: Repeat comment leader. When you press <Enter> in Insert mode, Vim automatically inserts the comment leader of the previous line on the new line. For example, if you are editing a C-style comment like // a comment, pressing enter will start the new line with //.
	
	o: Insert comment leader with 'o' and 'O'. When you use the o or O commands in Normal mode to create a new line, Vim automatically inserts the correct comment leader. 
	
	Usage notes:
	1. Start writing a block comment by typing /* and pressing Enter.
	2. Vim will automatically add the * prefix on the next line.
	3. When you are finished with the comment, press Enter followed by / to close the comment with */. 
	
3. set tabstop=4, is required and used by norminette to align variables and functions.
4. set shiftwidth=4, makes the indentation of the next line to be the indentation of the tab.


So, add to home as Hidden File a file using "vim .vimrc" containing:

set number
set formatoptions+=ro
set tabstop=4
set shiftwidth=4
filetype indent on
