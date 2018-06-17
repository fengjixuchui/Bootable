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

#ifndef BOOTABLE_TOKEN_H
#define BOOTABLE_TOKEN_H

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Enumerates a series of possible
 * token types in a config file.
 * @ingroup lang-api
 * */

enum bootable_token_type {
	/** No token type was specified. */
	BOOTABLE_TOKEN_NONE,
	/** An unknown character was found. */
	BOOTABLE_TOKEN_UNKNOWN,
	/** Token indicates the end of the file. */
	BOOTABLE_TOKEN_END,
	/** Token consists of whitespace, including
	 * newlines, spaces, and tabs. */
	BOOTABLE_TOKEN_WHITESPACE,
	/** Token is a comment */
	BOOTABLE_TOKEN_COMMENT,
	/** Token is a string made up of two single quotes. */
	BOOTABLE_TOKEN_SINGLE_QUOTE,
	/** Token is a string made up of two double quotes. */
	BOOTABLE_TOKEN_DOUBLE_QUOTE,
	/** Token is a single colon. */
	BOOTABLE_TOKEN_COLON,
	/** Token is an identifier. */
	BOOTABLE_TOKEN_IDENTIFIER,
	/** Token is a number .*/
	BOOTABLE_TOKEN_NUMERICAL,
	/** Token is a bracket. */
	BOOTABLE_TOKEN_BRACKET,
	/** Token is a comma */
	BOOTABLE_TOKEN_COMMA
};

/** Describes a token in a config
 * file source.
 * @ingroup lang-api
 * */

struct bootable_token {
	/** This indicates the token type. */
	enum bootable_token_type type;
	/** This contains the characters of the token. */
	const char *data;
	/** This indicates the number of characters in the token. */
	unsigned long int size;
	/** This indicates how many characters the token occupies
	 * in the source text. This member is useful for iterating
	 * the source code, because it can be used to increment the
	 * character index after a successfull parse. */
	unsigned long int width;
	/** The line in the source that the token starts at. */
	unsigned long int line;
	/** The column within the line that the token starts at. */
	unsigned long int column;
};

/** This initializes the token structure
 * to reasonable default values. This should
 * be called before a token is parsed.
 * @param token The token to initialize.
 * @ingroup lang-api
 * */

void bootable_token_init(struct bootable_token *token);

/** Converts the token to a string.
 * For tokens that are either double quoted
 * or single quoted strings, the quotes are
 * discarded.
 * @param token An initialized token structure.
 * @returns A string representation of the token.
 * This function will not return @ref bootable_null
 * if the token is empty, it will return an empty
 * string. @ref bootable_null is only returned on
 * a memory allocation failure.
 * @ingroup lang-api
 * */

char *bootable_token_to_string(const struct bootable_token *token);

/** This parses a token from source text.
 * @param token An initialized token structure.
 * @param source A null-terminated source text.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_token_parse(struct bootable_token *token, const char *source);

/** This is the end-of-file token. It is sometimes
 * returned by a function, instead of a null pointer,
 * when an index is out of bounds.
 * @ingroup lang-api
 * */

extern const struct bootable_token bootable_eof_token;

/** This is a token buffer, used for parsing
 * a series of tokens.
 * @ingroup lang-api
 * */

struct bootable_tokenbuf {
	/** The token array, terminated by an end of
	 * file token. */
	struct bootable_token *token_array;
	/** The number of tokens in the token array,
	 * including the end of file token. */
	unsigned long int token_count;
	/** The number of token slots allocated in
	 * the token array. */
	unsigned long int tokens_reserved;
	/** Indicates whether or not whitespace
	 * tokens are added when parsed. */
	unsigned char allowing_whitespace;
	/** Indicates whether or not comment
	 * tokens are added when parsed. */
	unsigned char allowing_comments;
};

/** Initializes a token buffer.
 * @param tokenbuf The token buffer to initialize.
 * @ingroup lang-api
 * */

void bootable_tokenbuf_init(struct bootable_tokenbuf *tokenbuf);

/** Frees memory of a token buffer.
 * @param tokenbuf An initialized token buffer.
 * @ingroup lang-api
 * */

void bootable_tokenbuf_done(struct bootable_tokenbuf *tokenbuf);

/** This function will allow comments to be added to the
 * buffer when source is being parsed.
 * @param tokenbuf An initialized token buffer.
 * @ingroup lang-api
 * */

void bootable_tokenbuf_accept_comments(struct bootable_tokenbuf *tokenbuf);

/** This function will allow whitespace to be added
 * to the buffer when source is being parsed.
 * @param tokenbuf An initialized token buffer.
 * @ingroup lang-api
 * */

void bootable_tokenbuf_accept_whitespace(struct bootable_tokenbuf *tokenbuf);

/** This function will disallow comments from being
 * added to the buffer when source is being parsed.
 * @param tokenbuf An initialized token buffer.
 * @ingroup lang-api
 * */

void bootable_tokenbuf_reject_comments(struct bootable_tokenbuf *tokenbuf);

/** This function will disallow whitespace from being
 * added to the buffer when source is being parsed.
 * @param tokenbuf An initialized token buffer.
 * @ingroup lang-api
 * */

void bootable_tokenbuf_reject_whitespace(struct bootable_tokenbuf *tokenbuf);

/** Parses tokens from a source.
 * @param tokenbuf An initialized token buffer.
 * @param source The source to parse tokens for.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_tokenbuf_parse(struct bootable_tokenbuf *tokenbuf, const char *source);

/** Pushes a token to the end of the token buffer.
 * @param tokenbuf An initialized token buffer.
 * @param token The token to push to the end of
 * the buffer. This should not be an end-of-file
 * token, or an unknown or 'none' token.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_tokenbuf_push(struct bootable_tokenbuf *tokenbuf, const struct bootable_token *token);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_TOKEN_H */
