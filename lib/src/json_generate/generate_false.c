/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_false.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 02:57:58 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/08/30 03:27:53 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "internal.h"

int	rj_generate_false(t_json const *const item, t_output_buf *const buf)
{
	char	*write_pos;

	if (!item || !buf)
		return (-1);
	write_pos = rj_ensure(buf, 6);
	if (!write_pos)
		return (-1);
	strcpy(write_pos, "false");
	return (0);
}
