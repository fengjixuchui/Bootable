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

/** @file dir.h API related to the Pure64 file system directory. */

#ifndef BOOTABLE_DIR_H
#define BOOTABLE_DIR_H

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bootable_file;
struct bootable_stream;

/** A directory in a Pure64 image.
 * */

struct bootable_dir {
	/** The number of characters in the directory name. */
	bootable_uint64 name_size;
	/** The number of subdirectories in the directory. */
	bootable_uint64 subdir_count;
	/** The number o files in the directory. */
	bootable_uint64 file_count;
	/** The directory name. */
	char *name;
	/** The subdirectories in the directory. */
	struct bootable_dir *subdirs;
	/** The files in the directory. */
	struct bootable_file *files;
};

/** Initializes a directory structure.
 * @param dir An uninitialized directory structure.
 * */

void bootable_dir_init(struct bootable_dir *dir);

/** Releases memory allocated by a directory.
 * @param dir An initialized directory structure.
 * */

void bootable_dir_free(struct bootable_dir *dir);

/** Serializes the directory to a stream.
 * @param dir An initialized directory structure.
 * @param out The stream to export the directory to.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_dir_export(struct bootable_dir *dir, struct bootable_stream *out);

/** Deserializes a directory from a stream.
 * @param dir An initialized directory structure.
 * @param in The stream to read the directory from.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_dir_import(struct bootable_dir *dir, struct bootable_stream *in);

/** Adds a file to the directory.
 * This function will fail if the name of the file exists.
 * @param dir An initialized directory structure.
 * @param name The name of the file.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_dir_add_file(struct bootable_dir *dir, const char *name);

/** Adds a subdirectory to the directory.
 * This function will fail if the name of the file exists.
 * @param dir An initialized directory structure.
 * @param name The name of the subdirectory.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_dir_add_subdir(struct bootable_dir *dir, const char *name);

/** Checks if a name exists in the directory as either a
 * file or a directory.
 * @param dir An initialized directory.
 * @param name The name to check for.
 * @returns True if the name is found, false if it is not.
 * */

bootable_bool bootable_dir_name_exists(const struct bootable_dir *dir, const char *name);

/** Sets the name of the directory.
 * @param dir An initialized directory.
 * @param name The new name of the directory.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_dir_set_name(struct bootable_dir *dir, const char *name);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_DIR_H */
