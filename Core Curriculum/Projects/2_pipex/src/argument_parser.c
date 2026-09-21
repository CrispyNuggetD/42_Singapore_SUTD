/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argument_parser.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 18:27:28 by hnah              #+#    #+#             */
/*   Updated: 2026/09/21 22:01:29 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/* Skip spaces/tabs; return 1 if characters remain, otherwise 0. */
static int	skip_separators(const char *command, size_t *i)
{
	while (command[*i] == ' ' || command[*i] == '\t')
		(*i)++;
	return (command[*i] != '\0');
}

/* Dual function; A NULL destination measures;
 * otherwise the same scan copies the argument. */
static int	scan_argument(const char *command, size_t *i,
		char *destination, size_t *length)
{
	char	the_quote;

	the_quote = 0;
	*length = 0;
	while (command[*i] && (the_quote
			|| (command[*i] != ' ' && command[*i] != '\t')))
	{
		if (!the_quote && (command[*i] == '\'' || command[*i] == '"'))
			the_quote = command[*i];
		else if (the_quote && command[*i] == the_quote)
			the_quote = 0;
		else
		{
			if (destination)
				destination[*length] = command[*i];
			(*length)++;
		}
		(*i)++;
	}
	if (destination)
		destination[*length] = '\0';
	return (-(the_quote != 0));
}

static int	count_arguments(const char *command, size_t *count)
{
	size_t	i;
	size_t	length;

	i = 0;
	*count = 0;
	while (skip_separators(command, &i))
	{
		if (scan_argument(command, &i, NULL, &length) < 0)
			return (-1);
		(*count)++;
	}
	return (0);
}

static int	fill_arguments(char **arguments, const char *command)
{
	size_t	i;
	size_t	start;
	size_t	length;
	size_t	arg_index;

	i = 0;
	arg_index = 0;
	while (skip_separators(command, &i))
	{
		start = i;
		if (scan_argument(command, &i, NULL, &length) < 0)
			return (-1);
		arguments[arg_index] = malloc(length + 1);
		if (!arguments[arg_index])
			return (-1);
		i = start;
		if (scan_argument(command, &i, arguments[arg_index], &length) < 0)
			return (-1);
		arg_index++;
	}
	return (0);
}

/* Quote grouping only: backslashes are literal; no shell expansions. */
char	**split_command_args(const char *command)
{
	size_t	argument_count;
	char	**arguments;

	if (!command || count_arguments(command, &argument_count) < 0)
	{
		errno = EINVAL;
		return (NULL);
	}
	arguments = ft_calloc(argument_count + 1, sizeof(char *));
	if (!arguments)
	{
		errno = ENOMEM;
		return (NULL);
	}
	if (fill_arguments(arguments, command) < 0)
	{
		ryker_ft_free_str_array(arguments);
		errno = ENOMEM;
		return (NULL);
	}
	return (arguments);
}
