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
static int	is_improper_int(char **str_move, int *count, int sign);

int	count_int_in_str(char *str, int *count, int *values)
{
	char	*str_move;
	int		sign;
	
	sign = 1;
	while (*str == ' ')
		str++;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	if (ft_isdigit(*str))
	{
		str_move = str;
		if (is_improper_int(&str_move, count, sign))
			return (ERROR);
		values[*count-1] = sign * ft_atoi(str);
		if (has_duplicates(count, values))
			return (ERROR);
		if (*str_move && count_int_in_str(str_move, count, values))
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
	looping_index = cur_compare - 1;
	while (looping_index >= 0)
	{
		if (values[looping_index] == values[cur_compare])
			return (ERROR);
		looping_index--;
	}
	return (SUCCESS);
}	
	
static int is_improper_int(char **str_move, int *count, int sign)
{
	int			digits;
	const char	*int_limit;

	digits = 0;
	if (sign > 0)
		int_limit = "2147483647";
	else
		int_limit = "2147483648";
	while (**str_move && ft_isdigit(**str_move))
	{
		digits++;
		if (digits > 10)
			return (ERROR);
		(*str_move)++;
	}
	if (digits == 10)
	{
		while (digits--)
		{
			if (*(str_move - digits) < int_limit[10 - digits])
				break;
			else if (*(str_move - digits) > int_limit[10 - digits])
				return (ERROR);
		}
	}
	if (!**str_move || **str_move == ' ')
	{
		(*count)++;
		while (**str_move == ' ')
			(*str_move)++;
		if (*count > 500)
			return (ERROR);
	}
	return (SUCCESS);
}