/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_string.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 22:18:21 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 09:00:00 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

int	rj_generate_string(t_json const *const item, t_output_buf *const buf)
{
	if (!item || !buf)
		return (-1);
	return (rj_generate_string_ptr(item->_.string_data, buf));
}
