# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ikawamuk <ikawamuk@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/30 03:18:23 by ikawamuk          #+#    #+#              #
#    Updated: 2026/09/11 14:24:15 by ikawamuk         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

LIB_NAME	:= rosetta_json
VERSION		:= 0.1.0

PREFIX		?= /usr/local
DESTDIR		?=

BUILD_DIR	:= build
NAME		:= $(BUILD_DIR)/lib$(LIB_NAME).a
PC_FILE		:= $(BUILD_DIR)/$(LIB_NAME).pc

INC_DIR		:= include
SRC_DIR		:= src
TEST_DIR	:= tests
OBJ_DIR		:= $(BUILD_DIR)/obj

CC			:= cc
CFLAGS		:= -std=c99 -Wall -Wextra -Werror -pedantic-errors
LDLIBS 		:= -lm
CPPFLAGS	:= -I $(INC_DIR) -I $(SRC_DIR) -MMD -MP
AR			:= ar
ARFLAGS		:= rcs
RM			:= rm -f

# ---------------------------------------------------------------- sources -- #

SRCS		:= \
	$(SRC_DIR)/json_delete.c \
	$(SRC_DIR)/json_parse/json_parse.c \
	$(SRC_DIR)/json_parse/json_new_item.c \
	$(SRC_DIR)/json_parse/json_error.c \
	$(SRC_DIR)/json_parse/parse_buf.c \
	$(SRC_DIR)/json_parse/parse_buf_peek.c \
	$(SRC_DIR)/json_parse/is_json_whitespace.c \
	$(SRC_DIR)/json_parse/parse_null.c \
	$(SRC_DIR)/json_parse/parse_true.c \
	$(SRC_DIR)/json_parse/parse_false.c \
	$(SRC_DIR)/json_parse/parse_number.c \
	$(SRC_DIR)/json_parse/number_length.c \
	$(SRC_DIR)/json_parse/parse_string.c \
	$(SRC_DIR)/json_parse/parse_char.c \
	$(SRC_DIR)/json_parse/json_array.c \
	$(SRC_DIR)/json_parse/parse_array.c \
	$(SRC_DIR)/json_parse/json_object.c \
	$(SRC_DIR)/json_parse/parse_object.c \
	$(SRC_DIR)/json_generate/json_generate.c \
	$(SRC_DIR)/json_generate/output_buf.c \
	$(SRC_DIR)/json_generate/json_generate.c \
	$(SRC_DIR)/json_generate/output_buf.c \
	$(SRC_DIR)/json_generate/generate_null.c \
	$(SRC_DIR)/json_generate/generate_true.c \
	$(SRC_DIR)/json_generate/generate_false.c \
	$(SRC_DIR)/json_generate/generate_number.c \
	$(SRC_DIR)/json_generate/generate_string.c \
	$(SRC_DIR)/json_generate/generate_string_ptr.c \
	$(SRC_DIR)/json_generate/generate_array.c \
	$(SRC_DIR)/json_generate/generate_object.c \
	$(SRC_DIR)/json_access/json_is_null.c \
	$(SRC_DIR)/json_access/json_is_true.c \
	$(SRC_DIR)/json_access/json_is_false.c \
	$(SRC_DIR)/json_access/json_is_number.c \
	$(SRC_DIR)/json_access/json_is_string.c \
	$(SRC_DIR)/json_access/json_is_array.c \
	$(SRC_DIR)/json_access/json_is_object.c \
	$(SRC_DIR)/json_access/json_get_number.c \
	$(SRC_DIR)/json_access/json_get_string.c \
	$(SRC_DIR)/json_access/json_get_array.c \
	$(SRC_DIR)/json_access/json_get_object.c \
	$(SRC_DIR)/utils/encode_utf8.c \
	$(SRC_DIR)/utils/hex_to_int.c \
	$(SRC_DIR)/utils/single_escaped_char_util.c \

TEST_RUNNER				:= ./$(TEST_DIR)/run_test.sh
TEST_GENERATE_RUNNER	:= ./$(TEST_DIR)/run_test_generate.sh
TEST_TRANSFORM_RUNNER	:= ./$(TEST_DIR)/run_test_transform.sh
TEST_TARGET				:= $(BUILD_DIR)/rosetta_json
TEST_GENERATE_TARGET	:= $(BUILD_DIR)/rosetta_json_generate
TEST_SRCS				:= $(TEST_DIR)/test_main.c $(TEST_DIR)/test_util.c
TEST_GENERATE_SRCS		:= $(TEST_DIR)/test_generate_main.c $(TEST_DIR)/test_util.c
TEST_SUITE				:= ../JSONTestSuite

OBJS		:= $(SRCS:%.c=$(OBJ_DIR)/%.o)
TEST_OBJS	:= $(TEST_SRCS:%.c=$(OBJ_DIR)/%.o)
TEST_GENERATE_OBJS	:= $(TEST_GENERATE_SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS		:= $(sort $(OBJS:.o=.d) $(TEST_OBJS:.o=.d) $(TEST_GENERATE_OBJS:.o=.d))

# ---------------------------------------------------------------- targets -- #

all: $(NAME)

$(NAME): $(OBJS)
	@mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $@ $^

test: $(TEST_TARGET)
	chmod +x $(TEST_RUNNER)
	$(TEST_RUNNER)

test-generate: $(TEST_TARGET) $(TEST_GENERATE_TARGET)
	chmod +x $(TEST_GENERATE_RUNNER)
	$(TEST_GENERATE_RUNNER)

test-transform: $(TEST_TARGET) $(TEST_GENERATE_TARGET)
	chmod +x $(TEST_TRANSFORM_RUNNER)
	$(TEST_TRANSFORM_RUNNER)

test-all:
	-$(MAKE) test
	-$(MAKE) test-generate
	-$(MAKE) test-transform

$(TEST_TARGET): $(TEST_OBJS) $(NAME) $(TEST_SUITE)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(TEST_OBJS) $(NAME) -o $@ $(LDLIBS)

$(TEST_GENERATE_TARGET): $(TEST_GENERATE_OBJS) $(NAME) $(TEST_SUITE)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(TEST_GENERATE_OBJS) $(NAME) -o $@ $(LDLIBS)

$(TEST_SUITE):
	git clone git@github.com:nst/JSONTestSuite.git $@

# デバッグビルド (シンボル + AddressSanitizer)
debug: CFLAGS += -g3 -fsanitize=address,undefined
debug: fclean $(TEST_TARGET) $(TEST_GENERATE_TARGET)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# libjson.a が公開する記号を json_ (公開API) と rj_ (内部) だけに保つ
check-symbols: $(NAME)
	@leaked=$$(nm $(NAME) | awk '$$2 == "T" {print $$3}' \
		| grep -v '^json_' | grep -v '^rj_'); \
	if [ -n "$$leaked" ]; then \
		echo "prefix のない公開記号:"; echo "$$leaked" | sed 's/^/  /'; \
		exit 1; \
	fi; \
	echo "OK: 全記号が json_ / rj_ prefix に準拠"

# pkg-config 用ファイル。PREFIX に追従させるため毎回生成する (.PHONY 指定)
$(PC_FILE):
	@mkdir -p $(dir $@)
	@printf '%s\n' \
		'prefix=$(PREFIX)' \
		'exec_prefix=$${prefix}' \
		'libdir=$${exec_prefix}/lib' \
		'includedir=$${prefix}/include' \
		'' \
		'Name: $(LIB_NAME)' \
		'Description: JSON parser, generator and accessors' \
		'Version: $(VERSION)' \
		'Libs: -L$${libdir} -l$(LIB_NAME) -lm' \
		'Cflags: -I$${includedir}' > $@

install: $(NAME) $(PC_FILE)
	install -d $(DESTDIR)$(PREFIX)/lib/pkgconfig
	install -d $(DESTDIR)$(PREFIX)/include/$(LIB_NAME)
	install -m 644 $(NAME) $(DESTDIR)$(PREFIX)/lib/
	install -m 644 $(INC_DIR)/$(LIB_NAME)/json.h \
		$(DESTDIR)$(PREFIX)/include/$(LIB_NAME)/
	install -m 644 $(PC_FILE) $(DESTDIR)$(PREFIX)/lib/pkgconfig/

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/lib/lib$(LIB_NAME).a
	$(RM) $(DESTDIR)$(PREFIX)/lib/pkgconfig/$(LIB_NAME).pc
	$(RM) -r $(DESTDIR)$(PREFIX)/include/$(LIB_NAME)

clean:
	$(RM) -r $(OBJ_DIR)

fclean:
	$(RM) -r $(BUILD_DIR)

re: fclean all

-include $(DEPS)

.PHONY: all test test-generate test-transform test-all check-symbols debug \
	install uninstall $(PC_FILE) clean fclean re
