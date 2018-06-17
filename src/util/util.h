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

#ifndef BOOTABLE_UTIL_H
#define BOOTABLE_UTIL_H

#include <bootable/core/fs.h>
#include <bootable/lang/config.h>

#include "fstream.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bootable_uuid;

struct bootable_util {
	struct bootable_config config;
	/** The file associated with the
	 * Pure64 disk image. */
	struct bootable_fstream disk_file;
	/** The file system contained on
	 * the disk image. */
	struct bootable_fs fs;
	/** The standard error output of
	 * the utility. */
	FILE *errlog;
};

/** Initializes the utility.
 * @param util An uninitialized utilty structure.
 * */

void bootable_util_init(struct bootable_util *util);

/** Releases resources allocated by
 * the utility structure.
 * @param util An initialized utility structure.
 * */

void bootable_util_done(struct bootable_util *util);

/** Creates a new disk image.
 * @param util An initialized utility structure.
 * @param path The path to create the disk image at.
 * @returns Zero on success, an error code on failure.
 * */

int bootable_util_create_disk(struct bootable_util *util,
                            const char *path);

/** Opens an existing disk image.
 * @param util An initialized utility structure.
 * @param path The path of the disk image.
 * @returns Zero on success, an error code on failure.
 */

int bootable_util_open_disk(struct bootable_util *util,
                          const char *path);

/** Opens a configuration file.
 * @param util An initialized utility structure.
 * @param path The path of the config file.
 * @returns Zero on success, an error code on failure.
 * */

int bootable_util_open_config(struct bootable_util *util,
                            const char *path);

/** Saves information in memory onto the disk.
 * This should be called before @ref bootable_util_done,
 * if the changes made to the disk should remain.
 * This does not have to be called after creating
 * a disk.
 * @param util An initialized utility structure.
 * @returns Zero on success, an error code on failure.
 * */

int bootable_util_save_disk(struct bootable_util *util);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_UTIL_H */
