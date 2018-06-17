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

#ifndef BOOTABLE_LANG_PARSER_H
#define BOOTABLE_LANG_PARSER_H

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bootable_syntax_error;
struct bootable_var;

/** Used for parsing variables.
 * @ingroup lang-api
 * */

struct bootable_parser {
	/** The variable array. */
	struct bootable_var *var_array;
	/** The number of variables in
	 * variable array. */
	bootable_size var_count;
	/** The current variable index. */
	bootable_size var_index;
};

/** Initializes a parser structure.
 * @param parser The parser structure
 * to initialize.
 * @ingroup lang-api
 * */

void bootable_parser_init(struct bootable_parser *parser);

/** Releases memory allocated by a parser structure.
 * @param parser An initialized parser structure.
 * @ingroup lang-api
 * */

void bootable_parser_done(struct bootable_parser *parser);

/** Rewinds the parser to the first variable
 * that was parsed.
 * @param parser An initialized parser structure.
 * @ingroup lang-api
 * */

void bootable_parser_begin(struct bootable_parser *parser);

/** Indicates when the last variable
 * has been reached by the caller.
 * @param parser An initialized parser structure.
 * @returns @ref bootable_true if the last variable
 * was reached, @ref bootable_false if it was not.
 * @ingroup lang-api
 * */

bootable_bool bootable_parser_eof(const struct bootable_parser *parser);

/** Goes to the next variable found by the parser.
 * @param parser An initialized parser structure.
 * @returns A pointer to the next variable found
 * by the parser. If the last variable was already
 * reached, then @ref bootable_null is returned.
 * @ingroup lang-api
 * */

const struct bootable_var *bootable_parser_next(struct bootable_parser *parser);

/** Parses a configuration file source.
 * @param parser An initialized parser structure.
 * @param source The source to parse.
 * @param error A pointer to an error structure
 * that may be used to indicate the location
 * of a syntax error.
 * @returns Zero on success, an error code on failure.
 * The error code, @ref BOOTABLE_EINVAL is returned on
 * syntax errors.
 * @ingroup lang-api
 * */

int bootable_parser_parse(struct bootable_parser *parser,
                        const char *source,
                        struct bootable_syntax_error *error);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BOOTABLE_LANG_PARSER_H */
