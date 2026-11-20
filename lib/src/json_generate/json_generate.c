/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_generate.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 00:22:02 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 10:06:14 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include "internal.h"

static char	*generate(t_json const *const item, bool is_formatted);

char	*json_generate(const t_json *item)
{
	return (generate(item, true));
}

char	*json_generate_unformatted(const t_json *item)
{
	return (generate(item, false));
}

static char	*generate(t_json const *const item, bool is_formatted)
{
	char				*rev;
	t_output_buf		buf;

	rev = NULL;
	if (rj_output_buf_init(&buf, is_formatted) != 0)
		return (NULL);
	if (rj_generate_value(item, &buf) != 0)
	{
		free((void *)buf.content);
		return (NULL);
	}
	rj_update_offset(&buf);
	rev = realloc((void *)buf.content, buf.offset + 1);
	if (!rev)
	{
		free((void *)buf.content);
		return (NULL);
	}
	rev[buf.offset] = '\0';
	return (rev);
}

int	rj_generate_value(t_json const *const item, t_output_buf *const buf)
{
	if (!item || !buf)
		return (-1);
	if (item->type == JSON_Null)
		return (rj_generate_null(item, buf));
	if (item->type == JSON_True)
		return (rj_generate_true(item, buf));
	if (item->type == JSON_False)
		return (rj_generate_false(item, buf));
	if (item->type == JSON_Number)
		return (rj_generate_number(item, buf));
	if (item->type == JSON_String)
		return (rj_generate_string(item, buf));
	if (item->type == JSON_Array)
		return (rj_generate_array(item, buf));
	if (item->type == JSON_Object)
		return (rj_generate_object(item, buf));
	return (-1);
}
