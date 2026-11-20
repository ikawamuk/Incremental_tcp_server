/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_error.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 00:11:36 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/11 14:15:12 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

static t_json_error	g_error;

void	rj_set_error(size_t pos, t_error_code code)
{
	g_error.position = pos;
	g_error.code = code;
}

void	rj_error_reset(void)
{
	rj_set_error(-1, NO_ERROR);
}

int	json_get_error_code(void)
{
	return (g_error.code);
}

size_t	json_get_error_position(void)
{
	return (g_error.position);
}

char	*json_get_error_message(int error_code)
{
	if (error_code == NO_ERROR)
		return ("No error occurred");
	if (error_code == FAILED_TO_MEMORY_ALLOCATION)
		return ("Failed to memmory allocation");
	if (error_code == INVALID_TOKEN)
		return ("Invalid token");
	if (error_code == NESTING_IS_TOO_DEEP)
		return ("Nesting is too deep");
	return ("Unknown error occurred");
}
