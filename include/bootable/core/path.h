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

/** @file path.h API related to a file system path. */

#ifndef BOOTABLE_PATH_H
#define BOOTABLE_PATH_H

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A file or directory name that
 * is part of a path structure.
 * */

struct bootable_path_name {
	/** The name characters */
	char *data;
	/** The number of characters
	 * in the name. */
	bootable_uint64 size;
};

/** A file system path.
 * */

struct bootable_path {
	/** The directory name array */
	struct bootable_path_name *name_array;
	/** The number of names in the
	 * directory name array. */
	bootable_uint64 name_count;
};

/** Initializes a path structure.
 * @param path An uninitialized
 * path structure.
 * */

void bootable_path_init(struct bootable_path *path);

/** Releases resources allocated
 * by the path.
 * @param path An initialized path
 * structure.
 * */

void bootable_path_free(struct bootable_path *path);

/** Get a directory name from the path.
 * @param path An initialized path structure.
 * @param index The index of the directory
 * name within the path to get.
 * @returns The name of the directory
 * within the path. If the index
 * is out of bounds, null is returned.
 * */

const char *bootable_path_get_name(const struct bootable_path *path,
                                 bootable_uint64 index);

/** Get the number of directories specified in the path.
 * @param path An initialized path structure.
 * @returns The number of directories in the path.
 * */

bootable_uint64 bootable_path_get_name_count(const struct bootable_path *path);

/** Resolve '.' and '..' references.
 * If there are more '..' references than
 * there are real directory names, the root
 * directory will be leftover.
 * @param path An initialized path structure
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_path_normalize(struct bootable_path *path);

/** Parses a path string.
 * @param path An initialized path string.
 * @param path_string The string to parse
 * path from.
 * @returns Zero on success, non-zero on
 * failure.
 * */

int bootable_path_parse(struct bootable_path *path,
                      const char *path_string);

/** Add a file or directory name to the
 * end of the path.
 * @param path An initialized path structure.
 * @param name The file or directory name to push.
 * @returns Zero on success, non-zero otherwise.
 * */

int bootable_path_push_child(struct bootable_path *path,
                           const char *name);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_PATH_H */
