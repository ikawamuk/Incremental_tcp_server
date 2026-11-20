/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_array.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 01:54:25 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 09:43:50 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include "internal.h"

t_json_array	*rj_array_new_element(void)
{
	t_json_array	*array_node;

	array_node = malloc(sizeof(t_json_array));
	if (!array_node)
		return (NULL);
	memset(array_node, 0, sizeof(t_json_array));
	array_node->element = malloc(sizeof(t_json));
	if (!array_node->element)
	{
		free(array_node);
		return (NULL);
	}
	memset(array_node->element, 0, sizeof(t_json));
	return (array_node);
}

void	rj_array_delete(t_json_array *array)
{
	if (!array)
		return ;
	json_delete(array->element);
	rj_array_delete(array->next);
	free(array);
}
