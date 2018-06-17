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

#include <bootable/lang/var.h>

#include <bootable/core/error.h>
#include <bootable/core/string.h>

#include <bootable/lang/scanner.h>
#include <bootable/lang/syntax-error.h>
#include <bootable/lang/token.h>

#include <stdlib.h>

void bootable_object_init(struct bootable_object *object) {
	object->var_array = bootable_null;
	object->var_count = 0;
}

void bootable_object_done(struct bootable_object *object) {
	for (bootable_size i = 0; i < object->var_count; i++) {
		bootable_var_done(&object->var_array[i]);
	}
	free(object->var_array);
	object->var_array = NULL;
	object->var_count = 0;
}

int bootable_object_parse(struct bootable_object *object,
                        struct bootable_scanner *scanner,
                        struct bootable_syntax_error *error) {

	/* Check for '}' */

	bootable_size original_index = scanner->index;

	const struct bootable_token *bracket = bootable_scanner_next(scanner);
	if (bracket == bootable_null) {
		return BOOTABLE_EFAULT;
	} else if ((bracket->type == BOOTABLE_TOKEN_BRACKET)
	        && (bracket->data[0] == '}')) {
		return 0;
	} else {
		scanner->index = original_index;
	}

	for (;;) {

		/* Parse the variable. */

		struct bootable_var var;

		bootable_var_init(&var);

		int err = bootable_var_parse(&var, scanner, error);
		if (err != 0)
			return err;

		err = bootable_object_push(object, &var);
		if (err != 0)
			return err;

		/* Check for comma or bracket */

		original_index = scanner->index;

		const struct bootable_token *token = bootable_scanner_next(scanner);
		if (token == bootable_null)
			return BOOTABLE_EFAULT;
		else if (token->type == BOOTABLE_TOKEN_COMMA)
			continue;
		else if ((token->type == BOOTABLE_TOKEN_BRACKET)
		      && (token->data[0] == '}'))
			break;
	}

	return 0;
}

int bootable_object_push(struct bootable_object *object,
                       struct bootable_var *var) {

	bootable_size var_count = object->var_count + 1;

	struct bootable_var *var_array = object->var_array;

	var_array = realloc(var_array, sizeof(var_array[0]) * var_count);
	if (var_array == NULL)
		return BOOTABLE_ENOMEM;

	var_array[var_count - 1] = *var;

	object->var_array = var_array;
	object->var_count = var_count;

	return 0;
}

void bootable_list_init(struct bootable_list *list) {
	list->value_array = bootable_null;
	list->value_count = 0;
}

void bootable_list_done(struct bootable_list *list) {
	for (bootable_size i = 0; i < list->value_count; i++) {
		bootable_value_done(&list->value_array[i]);
	}
	free(list->value_array);
	list->value_array = NULL;
	list->value_count = 0;
}

int bootable_list_parse(struct bootable_list *list,
                      struct bootable_scanner *scanner,
                      struct bootable_syntax_error *error) {

	/* Check for ']' */

	bootable_size original_index = scanner->index;

	const struct bootable_token *bracket = bootable_scanner_next(scanner);
	if (bracket == bootable_null) {
		return BOOTABLE_EFAULT;
	} else if ((bracket->type == BOOTABLE_TOKEN_BRACKET)
	        && (bracket->data[0] == ']')) {
		return 0;
	} else {
		scanner->index = original_index;
	}

	for (;;) {

		/* Parse the value. */

		struct bootable_value value;

		bootable_value_init(&value);

		int err = bootable_value_parse(&value, scanner, error);
		if (err != 0)
			return err;

		err = bootable_list_push(list, &value);
		if (err != 0)
			return err;

		/* Check for comma or bracket */

		original_index = scanner->index;

		const struct bootable_token *token = bootable_scanner_next(scanner);
		if (token == bootable_null)
			return BOOTABLE_EFAULT;
		else if (token->type == BOOTABLE_TOKEN_COMMA)
			continue;
		else if ((token->type == BOOTABLE_TOKEN_BRACKET)
		      && (token->data[0] == ']'))
			break;
	}

	return 0;
}

int bootable_list_push(struct bootable_list *list,
                     struct bootable_value *value) {

	bootable_size value_count = list->value_count + 1;

	struct bootable_value *value_array = list->value_array;

	value_array = realloc(value_array, sizeof(value_array[0]) * value_count);
	if (value_array == NULL)
		return BOOTABLE_ENOMEM;

	value_array[value_count - 1] = *value;

	list->value_array = value_array;
	list->value_count = value_count;

	return 0;
}

void bootable_value_init(struct bootable_value *value) {
	value->type = BOOTABLE_VALUE_null;
	value->column = 1;
	value->line = 1;
}

void bootable_value_done(struct bootable_value *value) {
	/* TODO */
	switch (value->type) {
	case BOOTABLE_VALUE_null:
	case BOOTABLE_VALUE_boolean:
	case BOOTABLE_VALUE_number:
	case BOOTABLE_VALUE_string:
		break;
	case BOOTABLE_VALUE_list:
		bootable_list_done(&value->u.list);
		break;
	case BOOTABLE_VALUE_object:
		bootable_object_done(&value->u.object);
		break;
	}
	value->type = BOOTABLE_VALUE_null;
	value->column = 1;
	value->line = 1;
}

int bootable_value_parse(struct bootable_value *value,
                       struct bootable_scanner *scanner,
                       struct bootable_syntax_error *error) {

	const struct bootable_token *token = bootable_scanner_next(scanner);
	if (token == bootable_null) {
		if (error != bootable_null) {
			error->source = bootable_null;
			error->desc = "Unexpected end of file.";
			error->line = 0;
			error->column = 0;
		}
		return BOOTABLE_EINVAL;
	} else if (token->type == BOOTABLE_TOKEN_IDENTIFIER) {
		if ((token->size == 4)
		 && (token->data[0] == 't')
		 && (token->data[1] == 'r')
		 && (token->data[2] == 'u')
		 && (token->data[3] == 'e')) {
			value->type = BOOTABLE_VALUE_boolean;
			value->u._bool = bootable_true;
		} else if ((token->size == 4)
		        && (token->data[0] == 'n')
		        && (token->data[1] == 'u')
		        && (token->data[2] == 'l')
		        && (token->data[3] == 'l')) {
			value->type = BOOTABLE_VALUE_null;
		} else if ((token->size == 5)
		        && (token->data[0] == 'f')
		        && (token->data[1] == 'a')
		        && (token->data[2] == 'l')
		        && (token->data[3] == 's')
		        && (token->data[4] == 'e')) {
			value->type = BOOTABLE_VALUE_boolean;
			value->u._bool = bootable_false;
		} else {
			value->type = BOOTABLE_VALUE_string;
			value->u.string.data = token->data;
			value->u.string.size = token->size;
		}
	} else if ((token->type == BOOTABLE_TOKEN_SINGLE_QUOTE)
	        || (token->type == BOOTABLE_TOKEN_DOUBLE_QUOTE)) {
		value->type = BOOTABLE_VALUE_string;
		value->u.string.data = token->data;
		value->u.string.size = token->size;
	} else if (token->type == BOOTABLE_TOKEN_BRACKET) {
		if (token->data[0] == '{') {
			value->type = BOOTABLE_VALUE_object;
			bootable_object_init(&value->u.object);
			return bootable_object_parse(&value->u.object, scanner, error);
		} else if (token->data[0] == '[') {
			value->type = BOOTABLE_VALUE_list;
			bootable_list_init(&value->u.list);
			return bootable_list_parse(&value->u.list, scanner, error);
		} else {
			if (error != bootable_null) {
				error->source = bootable_null;
				error->desc = "Unexpected symbol.";
				error->line = token->line;
				error->column = token->column;
			}
			return BOOTABLE_EINVAL;
		}
	} else {
		if (error != bootable_null) {
			error->source = bootable_null;
			error->desc = "Unexpected token.";
			error->line = token->line;
			error->column = token->column;
		}
		return BOOTABLE_EINVAL;
	}

	return 0;
}

void bootable_key_init(struct bootable_key *key) {
	key->id = bootable_null;
	key->id_size = 0;
	key->column = 1;
	key->line = 1;
}

void bootable_key_done(struct bootable_key *key) {
	key->id = bootable_null;
	key->id_size = 0;
	key->column = 1;
	key->line = 1;
}

int bootable_key_cmp_id(const struct bootable_key *key,
                      const char *id) {

	if (key->id == bootable_null) {
		if ((id == bootable_null) || (id[0] == 0))
			return 0;
		else
			return -1;
	} else if (id == bootable_null) {
		if (key->id[0] == 0)
			return 0;
		else
			return 1;
	}

	unsigned int i = 0;

	while ((i < key->id_size) && (id[i] != 0)) {
		if (key->id[i] > id[i])
			return 1;
		else if (key->id[i] < id[i])
			return -1;
		else
			i++;
	}

	if (i < key->id_size)
		return 1;
	else if (id[i] != 0)
		return -1;
	else
		return 0;
}

int bootable_key_parse(struct bootable_key *key,
                     struct bootable_scanner *scanner,
                     struct bootable_syntax_error *error) {

	bootable_size original_index = scanner->index;

	const struct bootable_token *token = bootable_scanner_next(scanner);
	if (token == bootable_null) {
		if (error != bootable_null) {
			error->source = bootable_null;
			error->desc = "End of file reached.";
			error->line = 0;
			error->column = 0;
		}
		scanner->index = original_index;
		return BOOTABLE_EINVAL;
	}

	if ((token->type == BOOTABLE_TOKEN_IDENTIFIER)
	 || (token->type == BOOTABLE_TOKEN_SINGLE_QUOTE)
	 || (token->type == BOOTABLE_TOKEN_DOUBLE_QUOTE)) {
		key->id = token->data;
		key->id_size = token->size;
		key->line = token->line;
		key->column = token->column;
	} else {
		if (error != bootable_null) {
			error->source = bootable_null;
			error->desc = "Expected an identifier or string.";
			error->line = token->line;
			error->column = token->column;
		}
		scanner->index = original_index;
		return BOOTABLE_EINVAL;
	}

	return 0;
}

int bootable_key_parse_s(struct bootable_key *key,
                       const char *source,
                       struct bootable_syntax_error *error) {

	struct bootable_scanner scanner;
	bootable_scanner_init(&scanner);
	bootable_scanner_reject_whitespace(&scanner);
	bootable_scanner_reject_comments(&scanner);

	int err = bootable_scanner_scan(&scanner, source);
	if (err != 0) {
		bootable_scanner_done(&scanner);
		return err;
	}

	err = bootable_key_parse(key, &scanner, error);
	if (err != 0) {
		bootable_scanner_done(&scanner);
		return err;
	}

	bootable_scanner_done(&scanner);

	return 0;
}

void bootable_var_init(struct bootable_var *var) {
	bootable_key_init(&var->key);
	bootable_value_init(&var->value);
}

void bootable_var_done(struct bootable_var *var) {
	bootable_key_done(&var->key);
	bootable_value_done(&var->value);
}

int bootable_var_cmp_id(const struct bootable_var *var,
                      const char *id) {

	return bootable_key_cmp_id(&var->key, id);
}

int bootable_var_parse(struct bootable_var *var,
                     struct bootable_scanner *scanner,
                     struct bootable_syntax_error *error) {

	int err = bootable_key_parse(&var->key, scanner, error);
	if (err != 0)
		return err;

	const struct bootable_token *colon_token = bootable_scanner_next(scanner);
	if (colon_token == bootable_null) {
		return BOOTABLE_EFAULT;
	} else if (colon_token->type != BOOTABLE_TOKEN_COLON) {
		if (error != bootable_null) {
			error->source = bootable_null;
			error->desc = "Expected a ':'.";
			error->line = colon_token->line;
			error->column = colon_token->column;
		}
		return BOOTABLE_EINVAL;
	}

	err = bootable_value_parse(&var->value, scanner, error);
	if (err != 0)
		return err;

	return 0;
}
