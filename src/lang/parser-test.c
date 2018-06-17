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

#include <bootable/lang/parser.h>
#include <bootable/lang/var.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void unit_test_key(const char *id, const char *source) {

	struct bootable_key key;

	bootable_key_init(&key);

	int err = bootable_key_parse_s(&key, source, bootable_null);
	assert(err == 0);

	assert(bootable_key_cmp_id(&key, id) == 0);

	bootable_key_done(&key);
}

static void test_key(void) {
	unit_test_key("k1", " k1 ");
	unit_test_key("k2", "\tk2\t");
	unit_test_key("k3", "\"k3\"");
	unit_test_key("k4", "\'k4\'");
}

static void test_parser(void) {

	const char source[] = " partitions : [       \n"
	                      "  {                   \n"
	                      "    name : rootfs,    \n"
	                      "    size : 2MiB       \n"
	                      "  },                  \n"
	                      "  {                   \n"
	                      "    name : null,      \n"
	                      "    size : null       \n"
	                      "  }                   \n"
	                      "]                     \n";

	struct bootable_parser parser;
	bootable_parser_init(&parser);

	int err = bootable_parser_parse(&parser, source, bootable_null);
	assert(err == 0);

	const struct bootable_var *var = bootable_parser_next(&parser);
	assert(var != bootable_null);

	var = bootable_parser_next(&parser);
	assert(var == bootable_null);

	bootable_parser_done(&parser);
}

int main(void) {
	test_key();
	test_parser();
	return EXIT_SUCCESS;
}
