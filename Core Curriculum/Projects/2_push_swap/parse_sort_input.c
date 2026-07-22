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

//		printf("check_4: '%c'\n", *str_moving);	

static int	has_duplicates(const int *count, const int *values);
static int	is_improper_int(char **str_moving, int *count, int pos);

int	count_int_in_str(char *str, int *count, int *values)
{
	char	*str_move;
	int		pos;
	
	pos = 1;
	while (*str == ' ')
		str++;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			pos = 0;
		str++;
	}
	if (ft_isdigit(*str))
	{
		str_move = str;
		if (is_improper_int(&str_move, count, pos))
			return (ERROR);
		values[*count-1] = ft_atoi(str);
		if (has_duplicates(count, values))
			return (ERROR);
		if (*str_move == ' ' && count_int_in_str(str_move, count, values))
			return (ERROR);
		return (SUCCESS);
	}
	return (ERROR);
}

static int	has_duplicates(const int *count, const int *values)
{
	int	cur_compare;
	int	looping_index;
	
	cur_compare = *count - 1;
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
	
static int is_improper_int(char **str_moving, int *count, int pos)
{
	int	num_digits;
	
	num_digits = 0;
	while (**str_moving && ft_isdigit(**str_moving))
	{
		num_digits++;
		if (num_digits > 10)
			break;
		else if (num_digits == 10)
		{
			if (*(*str_moving-1) > '4' || (pos && **str_moving > '7') || (!pos && **str_moving > '8'))
				return (ERROR);
		}
		(*str_moving)++;
	}
	if (!**str_moving || **str_moving == ' ')
	{
		(*count)++;
		if (*count <= 500)
			return (SUCCESS);
	}
	return (ERROR);
}