#include "push_swap.h"
// 20260723(Thu)05:57:24+08:00

void	debug_print_int_array(const int *array, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		printf("%d", array[i]);
		if (i < size - 1)
			printf(" ");
		i++;
	}
	printf("\n");
}