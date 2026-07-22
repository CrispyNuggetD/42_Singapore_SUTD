#include "push_swap.h"



/*


TODO:
Multiple int per argv



*/

/* int parse_input(input *input, char **argv)
{
	input->input_len = 
	input->sorted_input = malloc(sizeof(int) * (size + 1));
	return(SUCCESS);
} */

//(printf("oops_1\n"),

static int	check_duplicate(const int *count, const int *values);
static int	check_if_proper_int(char *str_moving, int *count, int pos, int *values);

int count_int_in_str(const char *str, int *count, int *values)
{
	char	*str_moving;
	int		pos;
	
	pos = 1;
	str_moving = (char *)str;
	if ((*str_moving && ft_isdigit(*str_moving)) || *str_moving == '+' || *str_moving == '-')
	{
		if (*str_moving == '-')
			pos = 0;
		while (*str_moving == '+' || *str_moving == '-')
			str_moving++;	
		if (!check_if_proper_int(str_moving, count, pos, values))
			return (ERROR);
		values[*count-1] = ft_atoi(str);
		if (!check_duplicate(count, values))
			return (ERROR);
		return (SUCCESS);
	}
	return (ERROR);
}

static int	check_duplicate(const int *count, const int *values)
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
}	
	
static int	check_if_proper_int(char *str_moving, int *count, int pos, int *values)
{
	int	num_digits;
	
	num_digits = 0;
	while (*str_moving && ft_isdigit(*str_moving))
	{
		num_digits++;

		if (num_digits > 10)
			break;
		else if (num_digits == 10)
		{
			if (*(str_moving-1) > '4' || (pos && *str_moving > '7') || (!pos && *str_moving > '8'))
				return (ERROR);
		}
		str_moving++;
	}
	if (!*str_moving || *str_moving == ' ')
	{
		(*count)++;
		if (*str_moving == ' ' && !count_int_in_str(str_moving + 1, count, values))
				return (ERROR);
		return (SUCCESS);
	}
	return (ERROR);
}