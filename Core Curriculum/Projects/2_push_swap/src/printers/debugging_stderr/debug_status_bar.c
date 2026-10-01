/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_status_bar.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/30 14:51:28 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

static void	append_text(t_status_line *line, const char *text)
{
	while (*text && line->length < 512)
		line->text[line->length++] = *text++;
}

/* Decimal conversion only; no allocation or printf parsing. */
static void	append_field(t_status_line *line, const char *label,
	unsigned long long value)
{
	char	digits[20];
	int		count;

	append_text(line, label);
	count = 0;
	while (value >= 10)
	{
		digits[count++] = '0' + value % 10;
		value /= 10;
	}
	digits[count++] = '0' + value;
	while (count > 0 && line->length < 512)
		line->text[line->length++] = digits[--count];
}

static void	status_bar(t_status_line *line, int percent, int width)
{
	int	i;

	append_text(line, " [");
	i = 0;
	while (++i <= width)
	{
		if (i * 1000 <= percent * width)
			append_text(line, "#");
		else
			append_text(line, "-");
	}
	append_text(line, "] ");
	append_field(line, "", percent / 10);
	append_field(line, ".", percent % 10);
	append_text(line, "%");
}

static void	status_fields(t_status_line *line, t_search_debug *s)
{
	append_field(line, "\r\033[2KA", s->algo_id);
	append_field(line, "/", ALGO_COUNT);
	append_field(line, " D", s->status_level + 1);
	append_field(line, "/", s->depth_limit);
	append_text(line, " E");
	status_bar(line, s->roots_done * 1000 / ryker_ft_max(1, s->initial_b), 5);
	append_text(line, " I");
	status_bar(line, s->percent_tenths, 15);
	append_field(line, " eval=", s->evaluated);
}

/* One bounded stack buffer and one stderr write per actual redraw. */
void	debug_status_draw(int complete)
{
	t_status_line	line;
	t_search_debug	*s;

	s = debug_search_state();
	if (DEBUG != 1 || !debug_status_ready(s, complete))
		return ;
	line.length = 0;
	status_fields(&line, s);
	write(STDERR_FILENO, line.text, line.length);
}
