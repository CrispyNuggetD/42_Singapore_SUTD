#include "push_swap.h"

/* int parse_input(input *input, char **argv)
{
	input->input_len = 
	input->sorted_input = malloc(sizeof(int) * (size + 1));
	return(SUCCESS);
} */

//		printf("check_4: '%c'\n", *str_moving);

int	rank_values(const int count, const int *values, int *ranks)
{
	int	i;
	int	j;
	int	pos;

	i = 0;
	{
		while (i < count - 1)
		{
			pos = 0;
			j = i + 1;
			while (j < count)
			{
				if (values[i] > values[j++])
					pos++;
			}
			ranks[i++] = pos;
		}
		return (SUCCESS);
	}
	return (ERROR);
}