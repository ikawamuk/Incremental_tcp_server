/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_new_item.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 00:13:58 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/03 09:15:53 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include "internal.h"

t_json	*rj_new_item(void)
{
	t_json	*item;

	item = malloc(sizeof(t_json));
	if (item)
		memset(item, 0, sizeof(t_json));
	return (item);
}
