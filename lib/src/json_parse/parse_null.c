/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_null.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 01:44:28 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/08/30 03:49:32 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "internal.h"

int	rj_parse_null(t_json *item, t_parse_buf *const buf)
{
	if (!rj_can_read_n_bytes(buf, 4)
		|| strncmp((const char *)rj_parse_buf_at_offset(buf), "null", 4) != 0)
	{
		rj_set_error(buf->offset, INVALID_TOKEN);
		return (-1);
	}
	item->type = JSON_Null;
	buf->offset += 4;
	return (0);
}
