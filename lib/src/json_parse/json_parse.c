/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_parse.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 00:21:17 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/04 17:38:07 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "internal.h"

static bool		parse_buf_is_eof(t_parse_buf *buf);
static t_json	*parse_document(t_json *item, t_parse_buf *const buf);

t_json	*json_parse(const char *json_text)
{
	t_json		*item;
	t_parse_buf	buffer;

	if (!json_text)
		return (NULL);
	rj_error_reset();
	item = rj_new_item();
	if (!item)
	{
		rj_set_error(0, FAILED_TO_MEMORY_ALLOCATION);
		return (NULL);
	}
	rj_parse_buf_init(&buffer, json_text);
	if (!parse_document(item, &buffer))
	{
		json_delete(item);
		return (NULL);
	}
	return (item);
}

static t_json	*parse_document(t_json *item, t_parse_buf *const buf)
{
	if (rj_parse_value(item, rj_parse_buf_skip_whitespace(buf)) != 0)
		return (NULL);
	rj_parse_buf_skip_whitespace(buf);
	if (!parse_buf_is_eof(buf))
	{
		rj_set_error(0, TRAILING_GARBAGE);
		return (NULL);
	}
	return (item);
}

int	rj_parse_value(t_json *item, t_parse_buf *const buf)
{
	char	c;

	if (!buf || !buf->content || !rj_can_access_at_index(buf, 0))
		return (-1);
	c = rj_parse_buf_at_offset(buf)[0];
	if (c == 'n')
		return (rj_parse_null(item, buf));
	else if (c == 't')
		return (rj_parse_true(item, buf));
	else if (c == 'f')
		return (rj_parse_false(item, buf));
	else if (c == '-' || isdigit(c))
		return (rj_parse_number(item, buf));
	else if (c == '"')
		return (rj_parse_string(item, buf));
	else if (c == '[')
		return (rj_parse_array(item, buf));
	else if (c == '{')
		return (rj_parse_object(item, buf));
	rj_set_error(buf->offset, INVALID_TOKEN);
	return (-1);
}

static bool	parse_buf_is_eof(t_parse_buf *buf)
{
	return (buf->offset >= buf->length);
}
