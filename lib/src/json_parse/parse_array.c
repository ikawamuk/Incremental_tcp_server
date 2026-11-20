/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_array.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 22:17:12 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 10:06:21 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

static int		parse_array_core(t_json_array **list, t_parse_buf *const buf);
static int		update_cur_node(t_json_array **head, t_json_array **cur);
static int		success(t_json *item, t_parse_buf *buf, t_json_array *head);
static int		fail(t_parse_buf *buf, t_json_array *array, t_error_code code);

int	rj_parse_array(t_json *item, t_parse_buf *const buf)
{
	t_json_array	*tmp_array;

	++buf->depth;
	if (buf->depth >= JSON_NESTING_LIMIT)
		return (fail(buf, NULL, NESTING_IS_TOO_DEEP));
	if (rj_parse_buf_at_offset(buf)[0] != '[')
		return (fail(buf, NULL, INVALID_TOKEN));
	++buf->offset;
	rj_parse_buf_skip_whitespace(buf);
	if (rj_buf_peek_is(buf, ']'))
		return (success(item, buf, NULL));
	if (!rj_can_access_at_index(buf, 0))
	{
		--buf->offset;
		return (fail(buf, NULL, INVALID_TOKEN));
	}
	--buf->offset;
	if (parse_array_core(&tmp_array, buf) != 0)
		return (-1);
	if (!rj_buf_peek_is(buf, ']'))
		return (fail(buf, tmp_array, INVALID_TOKEN));
	return (success(item, buf, tmp_array));
}

static int	parse_array_core(t_json_array **list, t_parse_buf *const buf)
{
	t_json_array	*cur;

	*list = NULL;
	cur = *list;
	while (1)
	{
		if (update_cur_node(list, &cur) != 0)
			return (fail(buf, *list, FAILED_TO_MEMORY_ALLOCATION));
		++buf->offset;
		if (rj_parse_value(cur->element,
				rj_parse_buf_skip_whitespace(buf)) != 0)
			return (fail(buf, *list, json_get_error_code()));
		rj_parse_buf_skip_whitespace(buf);
		if (!rj_buf_peek_is(buf, ','))
			break ;
	}
	return (0);
}

static int	update_cur_node(t_json_array **head, t_json_array **cur)
{
	t_json_array	*next;

	next = rj_array_new_element();
	if (!next)
		return (-1);
	if (!*head)
	{
		*head = next;
		*cur = next;
	}
	else
	{
		(*cur)->next = next;
		next->prev = *cur;
		*cur = next;
	}
	return (0);
}

static int	fail(t_parse_buf *buf, t_json_array *array, t_error_code code)
{
	rj_array_delete(array);
	rj_set_error(buf->offset, code);
	--buf->depth;
	return (-1);
}

static int	success(t_json *item, t_parse_buf *buf, t_json_array *head)
{
	--buf->depth;
	item->type = JSON_Array;
	item->_.array_data = head;
	++buf->offset;
	return (0);
}
