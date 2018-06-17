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

#ifndef BOOTABLE_SCANNER_H
#define BOOTABLE_SCANNER_H

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bootable_token;
struct bootable_tokenbuf;

/** A token scanner. Contains a token buffer
 * and allows the caller to go through the tokens
 * in the token buffer in a clean manner.
 * @ingroup lang-api
 * */

struct bootable_scanner {
	/** The token buffer. */
	struct bootable_tokenbuf *tokenbuf;
	/** The token index within the token buffer. */
	bootable_size index;
};

/** Initializes the scanner structure.
 * @param scanner The scanner structure to initialize.
 * @ingroup lang-api
 * */

void bootable_scanner_init(struct bootable_scanner *scanner);

/** Releases memory allocated by the scanner structure.
 * @param scanner An initialized scanner structure.
 * @ingroup lang-api
 * */

void bootable_scanner_done(struct bootable_scanner *scanner);

/** Discards comments when tokenizing the source.
 * @param scanner An initialized scanner structure.
 * @ingroup lang-api
 * */

void bootable_scanner_reject_comments(struct bootable_scanner *scanner);

/** Discards whitespace when tokenizing the source.
 * @param scanner An initialized scanner structure.
 * @ingroup lang-api
 * */

void bootable_scanner_reject_whitespace(struct bootable_scanner *scanner);

/** Indicates when the scanner has no more tokens left.
 * @param scanner An initialized scanner structure.
 * @returns @ref bootable_true if the scanner has no more
 * tokens left, @ref bootable_false if it does.
 * @ingroup lang-api
 * */

bootable_bool bootable_scanner_eof(const struct bootable_scanner *scanner);

/** Tokenizes the source code, appending new tokens to the
 * end of the token buffer.
 * @param scanner An initialized scanner structure.
 * @param source A null-terminated string containing
 * the source code to tokenize.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_scanner_scan(struct bootable_scanner *scanner,
                        const char *source);

/** Resets the token index back to zero.
 * This is useful if the source code needs to be
 * scanned from the beginning again. It can also
 * be used just to ensure that the scanner starts
 * at the first token.
 * @param scanner An initialized scanner structure.
 * @ingroup lang-api
 * */

void bootable_scanner_begin(struct bootable_scanner *scanner);

/** Returns the next token in the scanner's token
 * buffer. If no more tokens are left over, then
 * @ref bootable_null is returned.
 * @param scanner An initialized scanner structure.
 * @returns A pointer to the next scanner token on
 * success, @ref bootable_null if there are no more left.
 * @ingroup lang-api
 * */

const struct bootable_token *bootable_scanner_next(struct bootable_scanner *scanner);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_SCANNER_H */
