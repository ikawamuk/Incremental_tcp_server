/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_array.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 02:20:56 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 09:49:16 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

static int	generate_elements(t_json const *item, t_output_buf *const buf);

int	rj_generate_array(t_json const *const item, t_output_buf *const buf)
{
	char	*write_pos;

	if (!item || !buf)
		return (-1);
	if (buf->depth >= JSON_NESTING_LIMIT)
		return (-1);
	++buf->depth;
	write_pos = rj_ensure(buf, 1);
	if (!write_pos)
		return (-1);
	*write_pos = '[';
	++buf->offset;
	if (generate_elements(item, buf) != 0)
		return (-1);
	write_pos = rj_ensure(buf, 2);
	if (!write_pos)
		return (-1);
	*write_pos++ = ']';
	*write_pos = '\0';
	++buf->offset;
	--buf->depth;
	return (0);
}

static int	generate_elements(t_json const *item, t_output_buf *const buf)
{
	t_json_array	*cur;

	cur = item->_.array_data;
	while (cur)
	{
		if (rj_generate_value(cur->element, buf) != 0)
			return (-1);
		rj_update_offset(buf);
		if (cur->next)
			if (rj_generate_comma(buf) != 0)
				return (-1);
		cur = cur->next;
	}
	return (0);
}

int	rj_generate_comma(t_output_buf *const buf)
{
	char	*write_pos;
	size_t	length;

	if (buf->is_formatted)
		length = 2;
	else
		length = 1;
	write_pos = rj_ensure(buf, length + 1);
	if (!write_pos)
		return (-1);
	*write_pos++ = ',';
	if (buf->is_formatted)
		*write_pos++ = ' ';
	*write_pos = '\0';
	buf->offset += length;
	return (0);
}
