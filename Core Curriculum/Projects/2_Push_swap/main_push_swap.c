#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
//#include "mylibrary.h"

int	main(int argc, char **argv)
{
	int temp_error_placeholder = 67;
	char** temp_argv_error_cases = argv;
	if (argc != 2 || argv == NULL)
		return (temp_error_placeholder);
	else
		printf("Hello World! It's: %s", *(temp_argv_error_cases + 1));
	return (0);
}