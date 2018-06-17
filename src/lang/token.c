/* Copyright (C) 2018 Taylor Holberton
 *
 * This file is part of Bootable.
 *
 * Bootable is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Bootable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bootable. If not, see <http://www.gnu.org/licenses/>.
 */

#include <bootable/lang/token.h>

#include <bootable/core/error.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/** The position of the scanner
 * within the source code.
 * */

struct source_pos {
	/** Indicates the line number, starting at one. */
	unsigned long int line;
	/** Indicates the column, starting at one. */
	unsigned long int column;
};

static void source_pos_init(struct source_pos *pos) {
	pos->line = 1;
	pos->column = 1;
}

static void source_pos_update(struct source_pos *pos,
                              const char *str,
                              unsigned long int size) {

	unsigned long int i = 0;

	while (i < size) {
		char c = str[i];
		if (c == '\n') {
			pos->line++;
			pos->column = 1;
		} else {
			pos->column++;
		}
		i++;
	}
}

const struct bootable_token bootable_eof_token = {
	BOOTABLE_TOKEN_END /* token type */,
	NULL /* data */,
	0 /* size */,
	0 /* width */,
	0 /* line */,
	0 /* column */
};

static int parse_whitespace(struct bootable_token *token, const char *source) {

	unsigned long int i = 0;

	while (source[i] != 0) {
		if ((source[i] != ' ')
		 && (source[i] != '\t')
		 && (source[i] != '\r')
		 && (source[i] != '\n')) {
			break;
		}
		i++;
	}

	if (i > 0) {
		token->type = BOOTABLE_TOKEN_WHITESPACE;
		token->data = source;
		token->size = i;
		token->width = i;
		return 0;
	}

	return BOOTABLE_EINVAL;
}

static int parse_comment(struct bootable_token *token, const char *source) {

	if (source[0] != '#') {
		return BOOTABLE_EINVAL;
	}

	unsigned long int i = 1;

	while (source[i] != 0) {
		if (source[i] == '\n')
			break;
		i++;
	}

	token->type = BOOTABLE_TOKEN_COMMENT;
	token->data = &source[1];
	token->size = i - 1;
	token->width = i;

	return 0;
}

static int parse_single_quote(struct bootable_token *token, const char *source) {

	unsigned long int i = 0;

	if (source[i] != '\'') {
		return BOOTABLE_EINVAL;
	}

	for (i = 1; source[i] != 0; i++) {
		if (source[i] == '\'') {
			token->type = BOOTABLE_TOKEN_SINGLE_QUOTE;
			token->data = &source[1];
			token->size = i - 1;
			token->width = token->size + 2;
			return 0;
		}
	}

	return BOOTABLE_EINVAL;
}

static int parse_double_quote(struct bootable_token *token, const char *source) {

	unsigned long int i = 0;

	if (source[i] != '\"') {
		return BOOTABLE_EINVAL;
	}

	for (i = 1; source[i] != 0; i++) {
		if (source[i] == '\"') {
			token->type = BOOTABLE_TOKEN_DOUBLE_QUOTE;
			token->data = &source[1];
			token->size = i - 1;
			token->width = token->size + 2;
			return 0;
		}
	}

	return BOOTABLE_EINVAL;
}

static int parse_bracket(struct bootable_token *token, const char *source) {
	if ((source[0] == '{')
	 || (source[0] == '}')
	 || (source[0] == '[')
	 || (source[0] == ']')) {
		token->type = BOOTABLE_TOKEN_BRACKET;
		token->data = source;
		token->size = 1;
		token->width = 1;
		return 0;
	}

	return BOOTABLE_EINVAL;
}

static int parse_comma(struct bootable_token *token, const char *source) {

	if (source[0] == ',') {
		token->type = BOOTABLE_TOKEN_COMMA;
		token->data = source;
		token->size = 1;
		token->width = 1;
		return 0;
	}

	return BOOTABLE_EINVAL;
}

static int parse_colon(struct bootable_token *token, const char *source) {

	if (source[0] == ':') {
		token->type = BOOTABLE_TOKEN_COLON;
		token->data = source;
		token->size = 1;
		token->width = 1;
		return 0;
	}

	return BOOTABLE_EINVAL;
}

static int parse_identifier(struct bootable_token *token, const char *source) {

	char c = source[0];

	if (!isalpha(c) && (c != '_')) {
		return BOOTABLE_EINVAL;
	}

	unsigned int i = 1;

	while (source[i] != 0) {
		if (!isalnum(source[i]) && (source[i] != '_')) {
			break;
		}
		i++;
	}

	token->type = BOOTABLE_TOKEN_IDENTIFIER;
	token->data = source;
	token->size = i;
	token->width = i;

	return 0;
}

static int parse_numerical(struct bootable_token *token, const char *source) {

	char c = source[0];
	if (!((c >= '0') && (c <= '9'))) {
		return BOOTABLE_EINVAL;
	}

	unsigned int i = 1;

	while (source[i] != 0) {
		if (!isalnum(source[i]) && (source[i] != '_')) {
			break;
		}
		i++;
	}

	token->type = BOOTABLE_TOKEN_IDENTIFIER;
	token->data = source;
	token->size = i;
	token->width = i;

	return 0;
}

static int reserve_tokens(struct bootable_tokenbuf *tokenbuf) {

	unsigned long int next_size = 0;
	next_size += tokenbuf->tokens_reserved;
	next_size += 64;
	next_size *= sizeof(struct bootable_token);

	struct bootable_token *tmp = tokenbuf->token_array;

	tmp = realloc(tmp, next_size);
	if (tmp == NULL)
		return BOOTABLE_ENOMEM;

	tokenbuf->token_array = tmp;
	tokenbuf->tokens_reserved += 64;

	return 0;
}

void bootable_token_init(struct bootable_token *token) {
	token->type = BOOTABLE_TOKEN_NONE;
	token->data = NULL;
	token->size = 0;
	token->width = 0;
	token->line = 1;
	token->column = 1;
}

char *bootable_token_to_string(const struct bootable_token *token) {

	char *str = malloc(token->size + 1);
	if (str == bootable_null)
		return bootable_null;

	for (unsigned int i = 0; i < token->size; i++)
		str[i] = token->data[i];

	str[token->size] = 0;

	return str;
}

int bootable_token_parse(struct bootable_token *token, const char *source) {

	if ((parse_whitespace(token, source) == 0)
	 || (parse_comment(token, source) == 0)
	 || (parse_single_quote(token, source) == 0)
	 || (parse_double_quote(token, source) == 0)
	 || (parse_bracket(token, source) == 0)
	 || (parse_colon(token, source) == 0)
	 || (parse_comma(token, source) == 0)
	 || (parse_identifier(token, source) == 0)
	 || (parse_numerical(token, source) == 0)) {
		return 0;
	}

	if (source[0] == 0) {
		token->type = BOOTABLE_TOKEN_END;
		token->data = source;
		token->size = 0;
		token->width = 0;
	} else {
		token->type = BOOTABLE_TOKEN_UNKNOWN;
		token->data = source;
		token->size = 1;
		token->width = 1;
	}

	return 0;
}

void bootable_tokenbuf_init(struct bootable_tokenbuf *tokenbuf) {
	tokenbuf->token_array = NULL;
	tokenbuf->token_count = 0;
	tokenbuf->tokens_reserved = 0;
	tokenbuf->allowing_comments = 1;
	tokenbuf->allowing_whitespace = 1;
}

void bootable_tokenbuf_done(struct bootable_tokenbuf *tokenbuf) {
	free(tokenbuf->token_array);
	tokenbuf->token_array = NULL;
	tokenbuf->token_count = 0;
	tokenbuf->tokens_reserved = 0;
}

void bootable_tokenbuf_accept_comments(struct bootable_tokenbuf *tokenbuf) {
	tokenbuf->allowing_comments = 1;
}

void bootable_tokenbuf_accept_whitespace(struct bootable_tokenbuf *tokenbuf) {
	tokenbuf->allowing_whitespace = 1;
}

void bootable_tokenbuf_reject_comments(struct bootable_tokenbuf *tokenbuf) {
	tokenbuf->allowing_comments = 0;
}

void bootable_tokenbuf_reject_whitespace(struct bootable_tokenbuf *tokenbuf) {
	tokenbuf->allowing_whitespace = 0;
}

int bootable_tokenbuf_parse(struct bootable_tokenbuf *tokenbuf, const char *source) {

	unsigned long int i = 0;

	unsigned long int source_len = strlen(source);

	struct bootable_token token;

	struct source_pos pos;

	source_pos_init(&pos);

	while (i < source_len) {

		bootable_token_init(&token);

		int err = bootable_token_parse(&token, &source[i]);
		if (err != 0)
			return err;

		token.line = pos.line;
		token.column = pos.column;

		source_pos_update(&pos, &source[i], token.width);

		if ((token.type == BOOTABLE_TOKEN_WHITESPACE)
		 && (!tokenbuf->allowing_whitespace)) {
			i += token.width;
			continue;
		}

		if ((token.type == BOOTABLE_TOKEN_COMMENT)
		 && (!tokenbuf->allowing_comments)) {
			i += token.width;
			continue;
		}

		err = bootable_tokenbuf_push(tokenbuf, &token);
		if (err != 0)
			return err;

		i += token.width;
	}

	int err = bootable_tokenbuf_push(tokenbuf, &bootable_eof_token);
	if (err != 0)
		return err;

	return 0;
}

int bootable_tokenbuf_push(struct bootable_tokenbuf *tokenbuf, const struct bootable_token *token) {

	if ((tokenbuf->token_count + 1) >= tokenbuf->tokens_reserved) {
		int err = reserve_tokens(tokenbuf);
		if (err != 0)
			return err;
	}

	tokenbuf->token_array[tokenbuf->token_count++] = *token;

	return 0;
}
