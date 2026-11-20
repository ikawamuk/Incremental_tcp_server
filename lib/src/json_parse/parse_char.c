/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_char.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 17:54:22 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/04 18:13:11 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <string.h>
#include "internal.h"

static int	single_escaped(char *out, int *rdbyte, int *wrbyte, char literal);
static int	normal_char(char *out, int *rdbytes, int *wrbytes, char literal);
static int	utf16_char(char *output, int *rdbytes, int *wrbytes, char *literal);

int	rj_parse_char(char *output, int *rdbytes, int *wrbytes, char *literal)
{
	if ((unsigned char)*literal < 0x20)
		return (-1);
	if (*literal != '\\')
		return (normal_char(output, rdbytes, wrbytes, *literal));
	++literal;
	if (*literal == '\0')
		return (-1);
	if (strchr("\"\\/bfnrt", *literal))
		return (single_escaped(output, rdbytes, wrbytes, *literal));
	if (*literal == 'u')
		return (utf16_char(output, rdbytes, wrbytes, ++literal));
	return (-1);
}

static int	normal_char(char *output, int *rdbytes, int *wrbytes, char literal)
{
	*output = literal;
	*rdbytes = 1;
	*wrbytes = 1;
	return (0);
}

static int	single_escaped(char *out, int *rdbytes, int *wrbytes, char literal)
{
	char	parsed_char;

	if (rj_unescape_single_char(literal, &parsed_char) != 0)
		return (-1);
	*out = parsed_char;
	*rdbytes = 2;
	*wrbytes = 1;
	return (0);
}

static int	utf16_char(char *output, int *rdbytes, int *wrbytes, char *literal)
{
	int	written;
	int	val;
	int	digit;
	int	i;

	val = 0;
	i = 0;
	while (i < 4)
	{
		if (literal[i] == '\0')
			return (-1);
		digit = rj_hex_to_int(literal[i]);
		if (digit < 0)
			return (-1);
		val = (val << 4) | digit;
		++i;
	}
	written = rj_encode_utf8(output, val);
	if (written < 0)
		return (-1);
	*rdbytes = 6;
	*wrbytes = written;
	return (0);
}
