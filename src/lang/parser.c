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

#include <bootable/core/error.h>
#include <bootable/lang/scanner.h>
#include <bootable/lang/syntax-error.h>
#include <bootable/lang/token.h>
#include <bootable/lang/var.h>

#include <stdlib.h>

void bootable_parser_init(struct bootable_parser *parser) {
	parser->var_array = bootable_null;
	parser->var_count = 0;
	parser->var_index = 0;
}

void bootable_parser_done(struct bootable_parser *parser) {

	for (bootable_size i = 0; i < parser->var_count; i++)
		bootable_var_done(&parser->var_array[i]);

	free(parser->var_array);

	parser->var_array = bootable_null;
	parser->var_count = 0;
	parser->var_index = 0;
}

bootable_bool bootable_parser_eof(const struct bootable_parser *parser) {
	if (parser->var_index >= parser->var_count)
		return bootable_true;
	else
		return bootable_false;
}

const struct bootable_var *bootable_parser_next(struct bootable_parser *parser) {
	if (bootable_parser_eof(parser))
		return bootable_null;
	else
		return &parser->var_array[parser->var_index++];
}

static int push_var(struct bootable_parser *parser,
                    struct bootable_var *var) {

	bootable_size var_count = parser->var_count + 1;

	struct bootable_var *var_array = parser->var_array;

	var_array = realloc(var_array, sizeof(var_array[0]) * var_count);
	if (var_array == NULL)
		return BOOTABLE_ENOMEM;

	parser->var_array = var_array;
	parser->var_count = var_count;

	var_array[var_count - 1] = *var;

	return 0;
}

int bootable_parser_parse(struct bootable_parser *parser,
                        const char *source,
                        struct bootable_syntax_error *error) {

	struct bootable_scanner scanner;
	bootable_scanner_init(&scanner);
	bootable_tokenbuf_reject_whitespace(scanner.tokenbuf);
	bootable_tokenbuf_reject_comments(scanner.tokenbuf);

	int err = bootable_scanner_scan(&scanner, source);
	if (err != 0) {
		if (error != bootable_null) {
			error->source = "<string>";
			error->desc = "Failed to tokenize source.";
			error->line = 1;
			error->column = 1;
		}
		return err;
	}

	while (!bootable_scanner_eof(&scanner)) {

		struct bootable_var var;

		bootable_var_init(&var);

		err = bootable_var_parse(&var, &scanner, error);
		if (err != 0) {
			bootable_scanner_done(&scanner);
			return err;
		}

		err = push_var(parser, &var);
		if (err != 0) {
			bootable_var_done(&var);
			bootable_scanner_done(&scanner);
			return err;
		}
	}

	bootable_scanner_done(&scanner);

	return 0;
}
