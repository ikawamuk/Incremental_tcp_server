/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_object.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 02:20:56 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 10:06:06 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

static int	generate_members(t_json const *item, t_output_buf *const buf);
static int	generate_colon(t_output_buf *const buf);
static int	generate_new_line(t_output_buf *const buf);
static int	generate_indent(t_output_buf *const buf);

int	rj_generate_object(t_json const *const item, t_output_buf *const buf)
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
	*write_pos = '{';
	++buf->offset;
	if (generate_new_line(buf) != 0)
		return (-1);
	if (generate_members(item, buf) != 0)
		return (-1);
	--buf->depth;
	if (generate_indent(buf) != 0)
		return (-1);
	write_pos = rj_ensure(buf, 2);
	if (!write_pos)
		return (-1);
	*write_pos++ = '}';
	*write_pos = '\0';
	return (0);
}

static int	generate_members(t_json const *item, t_output_buf *const buf)
{
	t_json_object	*cur;

	cur = item->_.object_data;
	while (cur)
	{
		if (generate_indent(buf) != 0)
			return (0);
		if (rj_generate_string_ptr(cur->key, buf) != 0)
			return (-1);
		if (generate_colon(buf) != 0)
			return (-1);
		if (rj_generate_value(cur->value, buf) != 0)
			return (-1);
		rj_update_offset(buf);
		if (cur->next)
			if (rj_generate_comma(buf) != 0)
				return (-1);
		if (generate_new_line(buf) != 0)
			return (-1);
		cur = cur->next;
	}
	return (0);
}

static int	generate_indent(t_output_buf *const buf)
{
	char	*write_pos;
	size_t	i;

	if (!buf->is_formatted)
		return (0);
	write_pos = rj_ensure(buf, buf->depth);
	if (!write_pos)
		return (-1);
	i = 0;
	while (i < buf->depth)
	{
		*write_pos++ = '\t';
		++i;
	}
	buf->offset += buf->depth;
	return (0);
}

static int	generate_new_line(t_output_buf *const buf)
{
	char	*write_pos;

	if (!buf->is_formatted)
		return (0);
	write_pos = rj_ensure(buf, 1);
	if (!write_pos)
		return (-1);
	*write_pos++ = '\n';
	*write_pos = '\0';
	buf->offset += 1;
	return (0);
}

static int	generate_colon(t_output_buf *const buf)
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
	*write_pos++ = ':';
	if (buf->is_formatted)
		*write_pos++ = ' ';
	*write_pos = '\0';
	buf->offset += length;
	return (0);
}
