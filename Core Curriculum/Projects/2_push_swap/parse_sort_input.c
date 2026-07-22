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
	int		pos;
	
	pos = 1;
	str_moving = (char *)str;
	if ((*str_moving && ft_isdigit(*str_moving)) || *str_moving == '+' || *str_moving == '-')
	{
		values[*count] = ft_atoi(str_moving);
		if (check_duplicate(count, values) == ERROR)
			return (ERROR);
		if (*str_moving == '-')
			pos = 0;	
		while (*str_moving == '+' || *str_moving == '-')
			str_moving++;
		return (check_if_proper_int(str_moving, pos));
	}
	return (ERROR);
}

int	check_duplicate(const int *count, const int *values)
{
	int	cur_compare;
	int	looping_index;
	
	cur_compare = *count;
	while (cur_compare > 0)
	{
		looping_index = cur_compare - 1;
		while (looping_index >= 0)
		{
			if (values[looping_index] == values[cur_compare])
				return (ERROR);
			looping_index--;
		}
		cur_compare--;
	}
	return (SUCCESS);
	
int	check_if_proper_int(char *str_moving, int pos)
{
	int	num_digits;
	
	num_digits = 1;
	while (*str_moving && ft_isdigit(*str_moving))
	{
		if (num_digits > 10)
			break;
		else if (num_digits == 9 && *str_moving > 4)
			return (ERROR);
		else if (!pos && *str_moving > 7) || *str_moving > 8)
			return (ERROR);
		str_moving++;
		num_digits++;
	}
	if (!*str_moving)
	{
		(*count)++;
		return (SUCCESS);
	}
	return (ERROR);	
}	