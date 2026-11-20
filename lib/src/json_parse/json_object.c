/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_object.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 02:01:53 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 09:14:36 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdlib.h>
#include "internal.h"

t_json_object	*rj_object_new_member(void)
{
	t_json_object	*member;

	member = malloc(sizeof(t_json_object));
	if (!member)
		return (NULL);
	memset(member, 0, sizeof(t_json));
	member->value = rj_new_item();
	if (!member->value)
	{
		free(member);
		return (NULL);
	}
	return (member);
}

void	rj_object_delete(t_json_object *object)
{
	if (!object)
		return ;
	free(object->key);
	json_delete(object->value);
	rj_object_delete(object->next);
	free(object);
}

void	rj_object_merge_duplicate(t_json_object *list, t_json_object **cur)
{
	t_json_object	*dup;
	t_json_object	*prev;

	dup = list;
	while (dup && dup != *cur)
	{
		if (strcmp(dup->key, (*cur)->key) == 0)
			break ;
		dup = dup->next;
	}
	if (!dup || dup == *cur)
		return ;
	prev = (*cur)->prev;
	json_delete(dup->value);
	dup->value = (*cur)->value;
	(*cur)->value = NULL;
	prev->next = NULL;
	rj_object_delete(*cur);
	*cur = prev;
}
