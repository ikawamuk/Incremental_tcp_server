/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_string_ptr.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 08:59:32 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 10:06:01 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "internal.h"

static size_t	count_literal_length(const char *str);
static int		string_to_literal(char *dest, const char *src);

int	rj_generate_string_ptr(char *str, t_output_buf *const buf)
{
	char	*output;
	size_t	literal_length;

	literal_length = count_literal_length(str);
	if (literal_length == (size_t)-1)
		return (-1);
	output = rj_ensure(buf, literal_length + 3);
	if (!output)
		return (-1);
	output[0] = '"';
	if (string_to_literal(output + 1, str) != 0)
		return (-1);
	output[literal_length + 1] = '"';
	output[literal_length + 2] = '\0';
	buf->offset += (literal_length + 2);
	return (0);
}

static void	generate_utf16_hex(char *out, unsigned char c)
{
	static const char	hex[] = "0123456789abcdef";

	out[0] = '\\';
	out[1] = 'u';
	out[2] = '0';
	out[3] = '0';
	out[4] = hex[(c >> 4) & 0x0F];
	out[5] = hex[c & 0x0F];
}

static int	string_to_literal(char *dest, const char *src)
{
	if (!src)
		return (-1);
	while (*src)
	{
		if (strchr("\"\\\b\f\n\r\t", *src))
		{
			*dest++ = '\\';
			if (rj_escape_single_char(*src++, dest) != 0)
				return (-1);
			++dest;
		}
		else if ((unsigned char)*src < 0x20)
		{
			generate_utf16_hex(dest, *src++);
			dest += 6;
		}
		else
			*dest++ = *src++;
	}
	return (0);
}

static size_t	count_literal_length(const char *str)
{
	size_t	len;

	if (!str)
		return (-1);
	len = 0;
	while (*str)
	{
		if (strchr("\"\\\b\f\n\r\t", *str))
			len += 2;
		else if ((unsigned char)*str < 0x20)
			len += 6;
		else
			++len;
		++str;
	}
	return (len);
}
