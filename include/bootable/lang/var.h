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

#ifndef BOOTABLE_LANG_VAR_H
#define BOOTABLE_LANG_VAR_H

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bootable_scanner;
struct bootable_syntax_error;
struct bootable_token;
struct bootable_var;
struct bootable_value;

/** Enumerates a data type.
 * This is relevant in determining
 * the value type of a variable.
 * @ingroup lang-api
 * */

enum bootable_value_type {
	/** Indicates that there is no type. */
	BOOTABLE_VALUE_null,
	/** Indicates that the value is boolean. */
	BOOTABLE_VALUE_boolean,
	/** Indicates that the value is a string. */
	BOOTABLE_VALUE_string,
	/** Indicates that the value is a number. */
	BOOTABLE_VALUE_number,
	/** Indicates that the value is a list. */
	BOOTABLE_VALUE_list,
	/** Indicates that the value is an object. */
	BOOTABLE_VALUE_object
};

/** A list of values, contained by a variable.
 * @ingroup lang-api
 * */

struct bootable_list {
	/** A pointer to the value array. */
	struct bootable_value *value_array;
	/** The number of values in the value array. */
	bootable_size value_count;
};

/** Initializes a list structure.
 * @param list An uninitialized list structure.
 * @ingroup lang-api
 * */

void bootable_list_init(struct bootable_list *list);

/** Releases memory allocated by a list structure.
 * @param list An initialized list structure.
 * @ingroup lang-api
 * */

void bootable_list_done(struct bootable_list *list);

/** Copies a list structure.
 * @param dst The destination list structure.
 * This is the list that receives the copied data.
 * @param src The source list structure.
 * This is the list that gets copied.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_list_copy(struct bootable_list *dst,
                     const struct bootable_list *src);

/** Parses a list
 * @param list An initialized list structure.
 * @param scanner An initialized scanner structure.
 * @param error A structure to relay syntax error information.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_list_parse(struct bootable_list *list,
                      struct bootable_scanner *scanner,
                      struct bootable_syntax_error *error);

/** Pushes a value to the end of the list.
 * @param list An initialized list structure.
 * @param value The value to push to the list.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_list_push(struct bootable_list *list,
                     struct bootable_value *value);

/** Represents an object structure contained
 * within a configuration file. It contains a
 * series of variables, which can be objects
 * themselves.
 * @ingroup lang-api
 * */

struct bootable_object {
	/** An array of variables contained by the object. */
	struct bootable_var *var_array;
	/** The number of variables in the variable array. */
	bootable_size var_count;
};

/** Initializes an object structure.
 * @param object The object structure to initialize.
 * @ingroup lang-api
 * */

void bootable_object_init(struct bootable_object *object);

/** Releases memory allocated by the object structure.
 * @param object An initialized object structure.
 * @ingroup lang-api
 * */

void bootable_object_done(struct bootable_object *object);

/** Copies an object structure.
 * @param dst The destination object structure.
 * This is the object structure that receives the copied data.
 * @param src The source object structure.
 * This is the object structure that is copied.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_object_copy(struct bootable_object *dst,
                       const struct bootable_object *src);

/** Parses an object structure.
 * @param object An initialized object structure.
 * @param scanner An initialized scanner structure.
 * @param error A structure to relay syntax error information.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_object_parse(struct bootable_object *object,
                        struct bootable_scanner *scanner,
                        struct bootable_syntax_error *error);

/** Pushes a variable to the end of the object structure.
 * @param object An initialized object structure.
 * @param var The variable to push to the end of the object.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_object_push(struct bootable_object *object,
                       struct bootable_var *var);

/** Represents a variable value.
 * @ingroup lang-api
 * */

struct bootable_value {
	/** The type of variable value. */
	enum bootable_value_type type;
	/** The line that the value begins at. */
	unsigned int line;
	/** The column that the value begins at. */
	unsigned int column;
	/** The type-specific value data. */
	union {
		/** Object data, if the value is an object. */
		struct bootable_object object;
		/** List data, if the value is a list. */
		struct bootable_list list;
		/** String data, if the value is a string. */
		struct {
			/** The characters in the string.
			 * There is no null terminator. */
			const char *data;
			/** The number of characters in the string. */
			bootable_size size;
		} string;
		/** The number value, if the value is a number. */
		bootable_uint64 number;
		/** The boolean value, if the value is boolean. */
		bootable_bool _bool;
	} u;
};

/** Initializes the value structure.
 * @param value The value structure to initialize.
 * @ingroup lang-api
 * */

void bootable_value_init(struct bootable_value *value);

/** Releases memory allocated by the value structure.
 * @param value An initialized value structure.
 * @ingroup lang-api
 * */

void bootable_value_done(struct bootable_value *value);

/** Copies a variable value.
 * @param dst The destination value.
 * This is where the copied data will go to.
 * @param src The source value.
 * This is where the data is copied from.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_value_copy(struct bootable_value *dst,
                      const struct bootable_value *src);

/** Parses a value.
 * @param value An initialized value structure.
 * @param scanner An initialized scanner structure.
 * @param error A structure to relay information
 * about a syntax error.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_value_parse(struct bootable_value *value,
                       struct bootable_scanner *scanner,
                       struct bootable_syntax_error *error);

/** This is a variable key, used to
 * distinguish variables.
 * @ingroup lang-api
 * */

struct bootable_key {
	/** The ID of the variable. */
	const char *id;
	/** The number of characters in the ID. */
	bootable_size id_size;
	/** The line that the key started on. */
	unsigned int line;
	/** The column that the key started on. */
	unsigned int column;
};

/** Initializes a variable key for use.
 * @param key An uninitialized variable key.
 * @ingroup lang-api
 * */

void bootable_key_init(struct bootable_key *key);

/** Releases memory allocated by a variable key.
 * @param key An initialized variable key.
 * @ingroup lang-api
 * */

void bootable_key_done(struct bootable_key *key);

/** Compares the ID of the key to
 * another ID in a null-terminated string.
 * @param key An initialized key structure.
 * @param id The string ID to compare with
 * the key ID.
 * @returns One if the key ID is greater than
 * the string ID, negative one if the key ID
 * is less than the string ID, zero if they're equal.
 * @ingroup lang-api
 * */

int bootable_key_cmp_id(const struct bootable_key *key,
                      const char *id);

/** Parses a variable key.
 * @param key An initialized key structure.
 * @param scanner An initialized scanner structure.
 * @param error An error structure, in case there
 * is a syntax error.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_key_parse(struct bootable_key *key,
                     struct bootable_scanner *scanner,
                     struct bootable_syntax_error *error);

/** Parses a variable key, from a null-terminated string.
 * @param key An initialized key structure.
 * @param source A null-terminated source containing the key.
 * @param error An error structure in case there is a syntax error.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_key_parse_s(struct bootable_key *key,
                       const char *source,
                       struct bootable_syntax_error *error);

/** Used to contain a variable found
 * in a configuration file.
 * @ingroup lang-api
 * */

struct bootable_var {
	/** The name of the variable. */
	struct bootable_key key;
	/** The value of the variable. */
	struct bootable_value value;
};

/** Initializes a variable structure.
 * @param var The variable structure to initialize.
 * @ingroup lang-api
 * */

void bootable_var_init(struct bootable_var *var);

/** Releases memory allocated by a variable.
 * @param var An initialized variable structure.
 * @ingroup lang-api
 * */

void bootable_var_done(struct bootable_var *var);

/** Used to find a variable of a specified name.
 * @param var An initialized variable structure.
 * @param id A null-terminated variable name.
 * @returns One if the variable name is greater
 * than the name give, negative one if it is less
 * than the name given, and zero if they're equal.
 * @ingroup lang-api
 * */

int bootable_var_cmp_id(const struct bootable_var *var,
                      const char *id);

/** Parses a variable.
 * @param var An initialized variable structure.
 * @param scanner An initialized scanner structure.
 * @param error An error structure used to relay information
 * on where a syntax error has occured. May be @ref bootable_null.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_var_parse(struct bootable_var *var,
                     struct bootable_scanner *scanner,
                     struct bootable_syntax_error *error);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BOOTABLE_LANG_VAR_H */
