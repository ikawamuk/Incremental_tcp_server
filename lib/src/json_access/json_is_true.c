/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_is_true.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 22:39:08 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/02 22:39:10 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rosetta_json/json.h"

bool	json_is_true(t_json const *const item)
{
	return (item->type == JSON_True);
}
