/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_status_bar.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnah <hnah@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:01:57 by hnah              #+#    #+#             */
/*   Updated: 2026/09/29 18:43:55 by hnah             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "greedy_reinsertion.h"

static void	append_text(t_status_line *line, const char *text)
{
	while (*text && line->length < 256)
		line->text[line->length++] = *text++;
}

/* Decimal conversion only; no allocation or printf parsing. */
static void	append_number(t_status_line *line, unsigned long long value)
{
	char	digits[20];
	int		count;

	count = 0;
	while (value >= 10)
	{
		digits[count++] = '0' + value % 10;
		value /= 10;
	}
	digits[count++] = '0' + value;
	while (count > 0 && line->length < 256)
		line->text[line->length++] = digits[--count];
}

static void	append_field(t_status_line *line, const char *label,
	unsigned long long value)
{
	append_text(line, label);
	append_number(line, value);
}

static void	status_bar(t_status_line *line, t_search_debug *s)
{
	int	i;

	while (!s->pass_capped && s->pass_total && s->percent < 100
		&& s->pass_done >= s->threshold[s->percent + 1])
		s->percent++;
	append_text(line, " [");
	i = 0;
	while (++i <= 10)
	{
		if (i * 10 <= s->percent)
			append_text(line, "#");
		else
			append_text(line, "-");
	}
	append_text(line, "] ");
	if (s->pass_capped)
		append_text(line, "?");
	else
		append_number(line, s->percent);
	append_text(line, "%");
}

/* One bounded stack buffer and one stderr write per actual redraw. */
void	debug_status_draw(int complete)
{
	t_status_line	line;
	t_search_debug	*s;

	if (DEBUG != 1)
		return ;
	s = debug_search_state();
	line.length = 0;
	append_field(&line, "\r\033[2Ktrials=", s->pass_done);
	append_field(&line, "/", s->pass_total);
	if (s->pass_capped)
		append_text(&line, "+");
	status_bar(&line, s);
	append_field(&line, " algo=", s->algo_id);
	append_field(&line, "/", ALGO_COUNT);
	append_text(&line, s->algo_label);
	append_field(&line, " depth=", s->status_level + 1);
	append_field(&line, "/", s->depth_limit);
	append_field(&line, " done=", s->candidate[s->status_level]);
	append_field(&line, "/", s->initial_b - s->status_level);
	append_field(&line, " best_item=", s->best_index);
	append_field(&line, " best_total=", s->best_total);
	if (complete && s->initial_b == 1)
		append_text(&line, " DONE");
	write(STDERR_FILENO, line.text, line.length);
}
