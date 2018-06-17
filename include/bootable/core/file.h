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

/** @file file.h API related to files in the Pure64 file system. */

#ifndef BOOTABLE_FILE_H
#define BOOTABLE_FILE_H

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bootable_stream;

/** A Pure64 file.
 * */

struct bootable_file {
	/** The number of characters in the file name. */
	bootable_uint64 name_size;
	/** The number of bytes in the file data. */
	bootable_uint64 data_size;
	/** The name of the file. */
	char *name;
	/** The file data. */
	void *data;
};

/** Initializes a file structure.
 * @param file An uninitialized file structure.
 * */

void bootable_file_init(struct bootable_file *file);

/** Releases memory allocated by the file structure.
 * @param file An initialized file structure.
 * */

void bootable_file_free(struct bootable_file *file);

/** Serializes a file to a stream.
 * @param file An initialized file structure.
 * @param out The stream to export the file to.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_file_export(struct bootable_file *file, struct bootable_stream *out);

/** Deserializes a file from a stream.
 * @param file An initialized file structure.
 * @param in The stream to read the file from.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_file_import(struct bootable_file *file, struct bootable_stream *in);

/** Sets the name of the file.
 * @param file An initialized file structure.
 * @param name The new name of the file.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_file_set_name(struct bootable_file *file, const char *name);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_FILE_H */
