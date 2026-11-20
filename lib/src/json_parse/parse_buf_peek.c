/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_buf_peek.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 08:20:00 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/08 08:10:01 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "internal.h"

bool	rj_buf_peek_is(t_parse_buf const *const buf, char c)
{
	if (!rj_can_access_at_index(buf, 0))
		return (false);
	return (rj_parse_buf_at_offset(buf)[0] == c);
}
