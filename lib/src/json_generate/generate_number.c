/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_number.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 22:18:33 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 09:22:10 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include "internal.h"

static bool	is_nun(double n);
static bool	is_infinity(double n);

int	rj_generate_number(t_json const *const item, t_output_buf *const buf)
{
	double	d;
	char	tmp_buffer[26];
	int		length;
	char	*write_pos;

	if (!item || !buf)
		return (-1);
	memset(tmp_buffer, 0, sizeof(tmp_buffer));
	d = item->_.number_data;
	if (is_nun(d) || is_infinity(d))
		length = sprintf(tmp_buffer, "null");
	else if (d == floor(d) && d >= INT_MIN && d <= INT_MAX)
		length = sprintf(tmp_buffer, "%d", (int)d);
	else
		length = sprintf(tmp_buffer, "%.15g", d);
	if (length < 0 || length > (int) sizeof(tmp_buffer) - 1)
		return (-1);
	write_pos = rj_ensure(buf, length + 1);
	if (!write_pos)
		return (-1);
	strcpy(write_pos, tmp_buffer);
	write_pos[length] = '\0';
	buf->offset += length;
	return (0);
}

static bool	is_nun(double n)
{
	return (n != n);
}

static bool	is_infinity(double n)
{
	return (n == INFINITY || n == -INFINITY);
}
