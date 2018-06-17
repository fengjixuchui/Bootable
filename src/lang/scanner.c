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

#include <bootable/lang/scanner.h>

#include <bootable/lang/token.h>
#include <bootable/core/error.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void bootable_scanner_init(struct bootable_scanner *scanner) {

	scanner->tokenbuf = malloc(sizeof(struct bootable_tokenbuf));
	if (scanner->tokenbuf != NULL)
		bootable_tokenbuf_init(scanner->tokenbuf);

	scanner->index = 0;
}

void bootable_scanner_done(struct bootable_scanner *scanner) {
	if (scanner->tokenbuf != NULL) {
		bootable_tokenbuf_done(scanner->tokenbuf);
		free(scanner->tokenbuf);
		scanner->tokenbuf = NULL;
	}
	scanner->index = 0;
}

void bootable_scanner_reject_comments(struct bootable_scanner *scanner) {
	if (scanner->tokenbuf != NULL) {
		bootable_tokenbuf_reject_comments(scanner->tokenbuf);
	}
}

void bootable_scanner_reject_whitespace(struct bootable_scanner *scanner) {
	if (scanner->tokenbuf != NULL) {
		bootable_tokenbuf_reject_whitespace(scanner->tokenbuf);
	}
}

bootable_bool bootable_scanner_eof(const struct bootable_scanner *scanner) {

	if (scanner->tokenbuf == NULL)
		return bootable_true;

	if (scanner->index >= scanner->tokenbuf->token_count)
		return bootable_true;
	else if (scanner->tokenbuf->token_array[scanner->index].type == BOOTABLE_TOKEN_END)
		return bootable_true;
	else
		return bootable_false;
}

void bootable_scanner_begin(struct bootable_scanner *scanner) {
	scanner->index = 0;
}

int bootable_scanner_scan(struct bootable_scanner *scanner,
                        const char *source) {

	if (scanner->tokenbuf == NULL)
		return BOOTABLE_ENOMEM;

	return bootable_tokenbuf_parse(scanner->tokenbuf, source);
}

const struct bootable_token *bootable_scanner_next(struct bootable_scanner *scanner) {

	if (scanner->tokenbuf == NULL)
		return bootable_null;

	if (bootable_scanner_eof(scanner))
		return &bootable_eof_token;
	else
		return &scanner->tokenbuf->token_array[scanner->index++];
}
