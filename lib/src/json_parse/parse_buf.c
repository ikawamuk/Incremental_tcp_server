/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_buf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 02:26:05 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/04 17:40:22 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdbool.h>
#include "internal.h"

void	rj_parse_buf_init(t_parse_buf *self, const char *text)
{
	self->content = (const unsigned char *)text;
	self->length = strlen(text);
	self->offset = 0;
	self->depth = 0;
}

t_parse_buf	*rj_parse_buf_skip_whitespace(t_parse_buf *const buf)
{
	if (!buf || !buf->content)
		return (NULL);
	if (!rj_can_access_at_index(buf, 0))
		return (buf);
	while (rj_can_access_at_index(buf, 0)
		&& rj_is_json_whitespace(rj_parse_buf_at_offset(buf)[0]))
		buf->offset++;
	return (buf);
}

bool	rj_can_read_n_bytes(t_parse_buf const *const buf, size_t len)
{
	if (!buf || !buf->content)
		return (false);
	return (buf->offset + len <= buf->length);
}

bool	rj_can_access_at_index(t_parse_buf const *const buf, size_t index)
{
	if (!buf || !buf->content)
		return (false);
	return (buf->offset + index < buf->length);
}

char	*rj_parse_buf_at_offset(t_parse_buf const *const buf)
{
	return ((char *)(buf->content + buf->offset));
}
