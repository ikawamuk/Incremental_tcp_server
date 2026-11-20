/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_get_object.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 22:41:03 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 09:14:48 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "rosetta_json/json.h"

bool	json_has_object_item(t_json const *const object, const char *key)
{
	struct s_json_object	*cur;

	cur = object->_.object_data;
	while (cur)
	{
		if (cur->key && strcmp(cur->key, key) == 0 && cur->value)
			return (true);
		cur = cur->next;
	}
	return (false);
}

t_json	*json_get_object_item(t_json const *const object, const char *key)
{
	struct s_json_object	*cur;

	cur = object->_.object_data;
	while (cur)
	{
		if (cur->key && strcmp(cur->key, key) == 0 && cur->value)
			return (cur->value);
		cur = cur->next;
	}
	return (NULL);
}
