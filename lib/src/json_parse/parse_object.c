/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_object.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 22:17:12 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/08 08:50:52 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

static int		parse_object_core(t_json_object **list, t_parse_buf *const buf);
static int		update_cur_node(t_json_object **list, t_json_object **cur);
static int		success(t_json *item, t_parse_buf *buf, t_json_object *head);
static int		fail(t_parse_buf *buf, t_json_object *obj, t_error_code code);

int	rj_parse_object(t_json *item, t_parse_buf *const buf)
{
	t_json_object	*tmp_object;

	++buf->depth;
	if (buf->depth >= JSON_NESTING_LIMIT)
		return (fail(buf, NULL, NESTING_IS_TOO_DEEP));
	if (rj_parse_buf_at_offset(buf)[0] != '{')
		return (fail(buf, NULL, INVALID_TOKEN));
	++buf->offset;
	rj_parse_buf_skip_whitespace(buf);
	if (rj_buf_peek_is(buf, '}'))
		return (success(item, buf, NULL));
	if (!rj_can_access_at_index(buf, 0))
	{
		--buf->offset;
		return (fail(buf, NULL, INVALID_TOKEN));
	}
	--buf->offset;
	if (parse_object_core(&tmp_object, buf) != 0)
		return (-1);
	if (!rj_buf_peek_is(buf, '}'))
		return (fail(buf, tmp_object, INVALID_TOKEN));
	return (success(item, buf, tmp_object));
}

static int	parse_object_core(t_json_object **list, t_parse_buf *const buf)
{
	t_json_object	*cur;

	*list = NULL;
	cur = *list;
	while (1)
	{
		if (update_cur_node(list, &cur) != 0)
			return (fail(buf, *list, FAILED_TO_MEMORY_ALLOCATION));
		++buf->offset;
		if (rj_parse_string(cur->value, rj_parse_buf_skip_whitespace(buf)) != 0)
			return (fail(buf, *list, json_get_error_code()));
		cur->key = cur->value->_.string_data;
		cur->value->_.string_data = NULL;
		rj_parse_buf_skip_whitespace(buf);
		if (!rj_buf_peek_is(buf, ':'))
			return (fail(buf, *list, INVALID_TOKEN));
		++buf->offset;
		if (rj_parse_value(cur->value, rj_parse_buf_skip_whitespace(buf)) != 0)
			return (fail(buf, *list, json_get_error_code()));
		rj_object_merge_duplicate(*list, &cur);
		rj_parse_buf_skip_whitespace(buf);
		if (!rj_buf_peek_is(buf, ','))
			break ;
	}
	return (0);
}

static int	update_cur_node(t_json_object **list, t_json_object **cur)
{
	t_json_object	*next;

	next = rj_object_new_member();
	if (!next)
		return (-1);
	if (!*cur)
	{
		*list = next;
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

static int	fail(t_parse_buf *buf, t_json_object *obj, t_error_code code)
{
	rj_object_delete(obj);
	rj_set_error(buf->offset, code);
	--buf->depth;
	return (-1);
}

static int	success(t_json *item, t_parse_buf *buf, t_json_object *head)
{
	--buf->depth;
	item->type = JSON_Object;
	item->_.object_data = head;
	++buf->offset;
	return (0);
}
