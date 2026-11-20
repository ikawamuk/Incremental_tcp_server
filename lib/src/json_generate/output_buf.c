/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output_buf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 02:34:44 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 09:48:50 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "internal.h"

static size_t	calculate_new_buffer_size(size_t needed);

int	rj_output_buf_init(t_output_buf *self, bool is_formatted)
{
	static const size_t	default_buffer_size = 256;

	self->content = malloc(sizeof(char) * default_buffer_size);
	self->size = default_buffer_size;
	self->offset = 0;
	self->size = 0;
	self->depth = 0;
	self->is_formatted = is_formatted;
	if (!self->content)
		return (-1);
	return (0);
}

char	*rj_ensure(t_output_buf *self, size_t needed)
{
	char	*new_buf;
	size_t	new_size;

	new_buf = NULL;
	new_size = 0;
	if (!self || !self->content || INT_MAX < needed
		|| ((0 < self->size) && self->size <= self->offset))
		return (NULL);
	needed += self->offset + 1;
	if (needed <= self->size)
		return ((char *)self->content + self->offset);
	new_size = calculate_new_buffer_size(needed);
	if (new_size == (size_t)-1)
		return (NULL);
	new_buf = realloc((void *)self->content, new_size);
	if (!new_buf)
	{
		free((void *)self->content);
		self->content = NULL;
		self->size = 0;
		return (NULL);
	}
	self->content = (const unsigned char *)new_buf;
	self->size = new_size;
	return ((char *)self->content + self->offset);
}

void	rj_update_offset(t_output_buf *const self)
{
	char	*head;

	if (!self || !self->content)
		return ;
	head = (char *)(self->content + self->offset);
	self->offset += strlen(head);
}

static size_t	calculate_new_buffer_size(size_t needed)
{
	size_t	new_size;

	if ((INT_MAX / 2) < needed)
	{
		if (needed <= INT_MAX)
			new_size = INT_MAX;
		else
			return (-1);
	}
	else
		new_size = needed * 2;
	return (new_size);
}
