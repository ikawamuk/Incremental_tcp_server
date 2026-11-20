/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encode_utf8.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 16:00:05 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/07 18:47:03 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

int	rj_encode_utf8(char *out, unsigned int cp)
{
	if (cp <= 0x7F)
	{
		out[0] = (char)cp;
		return (1);
	}
	else if (cp <= 0x7FF)
	{
		out[0] = (char)(0xC0 | (cp >> 6));
		out[1] = (char)(0x80 | (cp & 0x3F));
		return (2);
	}
	else if (cp <= 0xFFFF)
	{
		out[0] = (char)(0xE0 | (cp >> 12));
		out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
		out[2] = (char)(0x80 | (cp & 0x3F));
		return (3);
	}
	return (-1);
}
