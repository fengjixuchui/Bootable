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

/** @file fs.h API related to the Pure64 file system. */

#ifndef BOOTABLE_FS_H
#define BOOTABLE_FS_H

#include <bootable/core/dir.h>

/** The correct value of a Pure64 file
 * system signature. This is a hexidecimal
 * version of the string "Pure64FS".
 * */

#define BOOTABLE_SIGNATURE 0x5346343665727550ULL

/** The sector that contains the file
 * system used by Pure64.
 * */

#define BOOTABLE_FS_SECTOR 59

#ifdef __cplusplus
extern "C" {
#endif

struct bootable_file;
struct bootable_stream;

/** Pure64 file system.
 * Used for storing kernels and
 * various files that assist in
 * the loading and execution of
 * an operating system.
 * */

struct bootable_fs {
	/** File system signature (see @ref BOOTABLE_SIGNATURE). */
	bootable_uint64 signature;
	/** The number of bytes occupied by the file system. This
	 * is useful for determining the amount of memory needed
	 * to load the file system. This value is calculed only
	 * when the file system is exported. */
	bootable_uint64 size;
	/** The root directory of the
	 * file system. */
	struct bootable_dir root;
};

/** Initializes a file system structure.
 * @param fs An uninitialized file system structure.
 * */

void bootable_fs_init(struct bootable_fs *fs);

/** Releases resources allocated by the file
 * system structure.
 * @param fs An initialized file system structure.
 * */

void bootable_fs_free(struct bootable_fs *fs);

/** Exports the file system to a stream.
 * @param fs An initialized file system structure.
 * @param out The stream to export the file system to.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_fs_export(struct bootable_fs *fs, struct bootable_stream *out);

/** Imports the file system from a stream.
 * @param fs An initialized file system structure.
 * @param in The stream to import the file system from.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_fs_import(struct bootable_fs *fs, struct bootable_stream *in);

/** Creates a file in the file system.
 * @param fs An initialized file system structure.
 * @param path The path of the file to create.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_fs_make_file(struct bootable_fs *fs, const char *path);

/** Creates a subdirectory.
 * @param fs An initialized file system structure.
 * @param path The path to the directory to create.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_fs_make_dir(struct bootable_fs *fs, const char *path);

/** Opens an existing file.
 * @param fs An initialized file system structure.
 * @param path The path of the file to open.
 * @returns Zero on success, non-zero on failure.
 * */

struct bootable_file *bootable_fs_open_file(struct bootable_fs *fs, const char *path);

/** Opens an existing directory.
 * @param fs An initialized file system structure.
 * @param path The path of the directory to open.
 * @returns Zero on success, non-zero on failure.
 * */

struct bootable_dir *bootable_fs_open_dir(struct bootable_fs *fs, const char *path);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_FS_H */
