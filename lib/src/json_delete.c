/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_delete.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 00:21:41 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 02:01:39 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "internal.h"

void	json_delete(t_json *item)
{
	if (!item)
		return ;
	if (item->type == JSON_String)
		free(item->_.string_data);
	else if (item->type == JSON_Array)
		rj_array_delete(item->_.array_data);
	else if (item->type == JSON_Object)
		rj_object_delete(item->_.object_data);
	free(item);
	return ;
}
