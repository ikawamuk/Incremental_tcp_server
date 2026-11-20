/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_get_array.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 22:40:36 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/02 23:53:50 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rosetta_json/json.h"

size_t	json_get_array_size(t_json const *const array)
{
	size_t				size;
	struct s_json_array	*cur;

	cur = array->_.array_data;
	if (!cur->element)
		return (0);
	size = 0;
	while (cur)
	{
		++size;
		cur = cur->next;
	}
	return (size);
}

t_json	*json_get_array_element(t_json const *const array, size_t index)
{
	size_t				i;
	struct s_json_array	*cur;

	cur = array->_.array_data;
	i = 0;
	while (cur)
	{
		if (i == index)
			return (cur->element);
		cur = cur->next;
		++i;
	}
	return (NULL);
}
