/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_true.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 01:44:28 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/08/30 03:49:35 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "internal.h"

int	rj_parse_true(t_json *item, t_parse_buf *const buf)
{
	if (!rj_can_read_n_bytes(buf, 4)
		|| strncmp((const char *)rj_parse_buf_at_offset(buf), "true", 4) != 0)
	{
		rj_set_error(buf->offset, INVALID_TOKEN);
		return (-1);
	}
	item->type = JSON_True;
	item->_.bool_data = true;
	buf->offset += 4;
	return (0);
}
