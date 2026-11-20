/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   single_escaped_char_util.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 18:08:15 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/07 18:47:03 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include "internal.h"

static const char	g_escaped_chars[] = "\"\\/bfntr";
static const char	g_actual_chars[] = "\"\\/\b\f\n\t\r";

int	rj_unescape_single_char(char escaped_char, char *out_char)
{
	size_t	i;

	i = 0;
	while (g_escaped_chars[i] != '\0')
	{
		if (g_escaped_chars[i] == escaped_char)
		{
			*out_char = g_actual_chars[i];
			return (0);
		}
		++i;
	}
	return (-1);
}

int	rj_escape_single_char(char unescaped_char, char *out_char)
{
	size_t	i;

	i = 0;
	while (g_actual_chars[i] != '\0')
	{
		if (g_actual_chars[i] == unescaped_char)
		{
			*out_char = g_escaped_chars[i];
			return (0);
		}
		++i;
	}
	return (-1);
}
