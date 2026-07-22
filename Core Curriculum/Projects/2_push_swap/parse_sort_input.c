#include "push_swap.h"

/* int parse_input(input *input, char **argv)
{
	input->input_len = 
	input->sorted_input = malloc(sizeof(int) * (size + 1));
	return(SUCCESS);
} */

int count_int_in_str(const char *str, int *count, int *values)
{
	char	*str_moving;
	
	str_moving = (char *)str;
	if ((*str_moving && ft_isdigit(*str_moving)) || *str_moving == '+' || *str_moving == '-')
	{
		values[*count] = ft_atoi(str_moving);
		if (check_duplicate(count, values) == ERROR)
			return (ERROR);
		while (*str_moving == '+' || *str_moving == '-')
			str_moving++;
		while (*str_moving && ft_isdigit(*str_moving))
			str_moving++;	
		if (!*str_moving)
		{
			(*count)++;
			return (SUCCESS);
		}
	}
	return (ERROR);
}

int	check_duplicate(const int *count, const int *values)
{
	int cur_compare;
	int looping_index;
	
	cur_compare = (int)*count;
	ft_putnbr_fd(1, 1);
	while (cur_compare > 0)
	{
		ft_putnbr_fd(2, 1);
		looping_index = cur_compare - 1;
		while (looping_index >= 0)
		{
			ft_putnbr_fd(3, 1);
			if ((int)values[looping_index] == (int)values[cur_compare])
				return (ERROR);
			looping_index--;
		}
		cur_compare--;
		ft_putnbr_fd(4, 1);
	}
	return (SUCCESS);
}