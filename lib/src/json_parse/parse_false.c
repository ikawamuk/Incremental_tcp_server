/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_false.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 01:44:28 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/08/30 03:53:05 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "internal.h"

int	rj_parse_false(t_json *item, t_parse_buf *const buf)
{
	if (!rj_can_read_n_bytes(buf, 5)
		|| strncmp((const char *)rj_parse_buf_at_offset(buf), "false", 5) != 0)
	{
		rj_set_error(buf->offset, INVALID_TOKEN);
		return (-1);
	}
	item->type = JSON_False;
	item->_.bool_data = false;
	buf->offset += 5;
	return (0);
}
