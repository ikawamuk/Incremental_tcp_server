/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internal.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 08:20:00 by ikawamuk          #+#    #+#             */
/*   Updated: 2026/09/08 08:10:01 by ikawamuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef INTERNAL_H
# define INTERNAL_H

# include <stdbool.h>
# include <stddef.h>
# include "rosetta_json/json.h"

/* ---------------------------------------------------------------- error -- */

typedef struct s_json_error
{
	size_t			position;
	t_error_code	code;
}	t_json_error;

void			rj_error_reset(void);
void			rj_set_error(size_t pos, t_error_code code);

/* ------------------------------------------------------------ parse buf -- */

typedef struct s_parse_buf
{
	const unsigned char	*content;
	size_t				length;
	size_t				offset;
	size_t				depth;
}	t_parse_buf;

void			rj_parse_buf_init(t_parse_buf *self, const char *text);
t_parse_buf		*rj_parse_buf_skip_whitespace(t_parse_buf *const buf);
bool			rj_can_read_n_bytes(t_parse_buf const *const buf, size_t len);
bool			rj_can_access_at_index(t_parse_buf const *const buf,
					size_t index);
char			*rj_parse_buf_at_offset(t_parse_buf const *const buf);
bool			rj_buf_peek_is(t_parse_buf const *const buf, char c);

/* ----------------------------------------------------------- output buf -- */

typedef struct s_output_buf
{
	const unsigned char	*content;
	size_t				size;
	size_t				offset;
	size_t				depth;
	bool				is_formatted;
}	t_output_buf;

int				rj_output_buf_init(t_output_buf *self, bool is_formatted);
char			*rj_ensure(t_output_buf *self, size_t needed);
void			rj_update_offset(t_output_buf *const self);

/* ---------------------------------------------------------------- value -- */

t_json			*rj_new_item(void);
t_json_array	*rj_array_new_element(void);
void			rj_array_delete(t_json_array *array);
t_json_object	*rj_object_new_member(void);
void			rj_object_delete(t_json_object *object);
void			rj_object_merge_duplicate(t_json_object *list,
					t_json_object **cur);

/* ---------------------------------------------------------------- parse -- */

int				rj_parse_value(t_json *item, t_parse_buf *const buf);
int				rj_parse_null(t_json *item, t_parse_buf *const buf);
int				rj_parse_true(t_json *item, t_parse_buf *const buf);
int				rj_parse_false(t_json *item, t_parse_buf *const buf);
int				rj_parse_number(t_json *item, t_parse_buf *const buf);
int				rj_parse_string(t_json *item, t_parse_buf *const buf);
int				rj_parse_array(t_json *item, t_parse_buf *const buf);
int				rj_parse_object(t_json *item, t_parse_buf *const buf);
int				rj_parse_char(char *output, int *rdbytes, int *wrbytes,
					char *literal);
size_t			rj_count_valid_number_length(t_parse_buf *buf);

/* ------------------------------------------------------------- generate -- */

int				rj_generate_value(t_json const *const item,
					t_output_buf *const buf);
int				rj_generate_null(t_json const *const item,
					t_output_buf *const buf);
int				rj_generate_true(t_json const *const item,
					t_output_buf *const buf);
int				rj_generate_false(t_json const *const item,
					t_output_buf *const buf);
int				rj_generate_number(t_json const *const item,
					t_output_buf *const buf);
int				rj_generate_string(t_json const *const item,
					t_output_buf *const buf);
int				rj_generate_array(t_json const *const item,
					t_output_buf *const buf);
int				rj_generate_object(t_json const *const item,
					t_output_buf *const buf);
int				rj_generate_string_ptr(char *str, t_output_buf *const buf);
int				rj_generate_comma(t_output_buf *const buf);

/* ----------------------------------------------------------------- util -- */

bool			rj_is_json_whitespace(char c);
int				rj_hex_to_int(char c);
int				rj_encode_utf8(char *out, unsigned int cp);
int				rj_escape_single_char(char unescaped_char, char *out_char);
int				rj_unescape_single_char(char escaped_char, char *out_char);

#endif
