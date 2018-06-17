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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void test_parse(void) {

	const char *source = "_id01: 'value 1'\n"
	                     "#comment\n"
	                     "_id02:\t\"value 2\"\n";

	struct bootable_tokenbuf tokenbuf;

	bootable_tokenbuf_init(&tokenbuf);
	bootable_tokenbuf_reject_comments(&tokenbuf);
	bootable_tokenbuf_reject_whitespace(&tokenbuf);

	int err = bootable_tokenbuf_parse(&tokenbuf, source);
	assert(err == 0);
	assert(tokenbuf.token_count == 7);

	assert(tokenbuf.token_array[0].type == BOOTABLE_TOKEN_IDENTIFIER);
	assert(tokenbuf.token_array[0].size == 5);
	assert(memcmp(tokenbuf.token_array[0].data, "_id01", 5) == 0);

	assert(tokenbuf.token_array[1].type == BOOTABLE_TOKEN_COLON);
	assert(tokenbuf.token_array[1].size == 1);
	assert(memcmp(tokenbuf.token_array[1].data, ":", 1) == 0);

	assert(tokenbuf.token_array[2].type == BOOTABLE_TOKEN_SINGLE_QUOTE);
	assert(tokenbuf.token_array[2].size == 7);
	assert(memcmp(tokenbuf.token_array[2].data, "value 1", 7) == 0);

	assert(tokenbuf.token_array[3].type == BOOTABLE_TOKEN_IDENTIFIER);
	assert(tokenbuf.token_array[3].size == 5);
	assert(memcmp(tokenbuf.token_array[3].data, "_id02", 5) == 0);

	assert(tokenbuf.token_array[4].type == BOOTABLE_TOKEN_COLON);
	assert(tokenbuf.token_array[4].size == 1);
	assert(memcmp(tokenbuf.token_array[4].data, ":", 1) == 0);

	assert(tokenbuf.token_array[5].type == BOOTABLE_TOKEN_DOUBLE_QUOTE);
	assert(tokenbuf.token_array[5].size == 7);
	assert(memcmp(tokenbuf.token_array[5].data, "value 2", 7) == 0);

	assert(tokenbuf.token_array[6].type == BOOTABLE_TOKEN_END);

	bootable_tokenbuf_done(&tokenbuf);
}

int main(void) {
	test_parse();
	return EXIT_SUCCESS;
}
