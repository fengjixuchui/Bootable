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

#ifndef BOOTABLE_FSTREAM_H
#define BOOTABLE_FSTREAM_H

#include <bootable/core/stream.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** This structure represents
 * a file stream. It is used to
 * format GPT images on a file,
 * as opposed to a disk.
 * */

struct bootable_fstream {
	/** The base stream structure */
	struct bootable_stream base;
	/** The file pointer for the
	 * stream. */
	FILE *file;
};

/** Initializes a file stream structure.
 * @param fstream The file stream structure
 * to initialize.
 * */

void bootable_fstream_init(struct bootable_fstream *fstream);

/** Closes the file, if there was one open.
 * @param fstream An initalized file stream.
 * */

void bootable_fstream_done(struct bootable_fstream *fstream);

/** Opens a file to be used in the file stream.
 * @param fstream An initialized file stream structure.
 * @param path The path of the file to open.
 * @param mode The mode to open the file in.
 * @returns Zero on success, an error code on failure.
 * */

int bootable_fstream_open(struct bootable_fstream *fstream,
                        const char *path,
                        const char *mode);

/** Resize the file.
 * This function only works if
 * the file is open for writing.
 * @param fstream An initialized
 * file structure.
 * @param size The new size of
 * the file.
 * @returns Zero on success, an
 * error code on failure.
 * */

int bootable_fstream_resize(struct bootable_fstream *fstream,
                          long int size);

/** Zeroes out data on the file stream.
 * @param fstream An initialized file stream.
 * @param count The number of bytes to zero.
 * @returns Zero on success, an error code on failure.
 * */

int bootable_fstream_zero(struct bootable_fstream *fstream,
                        unsigned long int count);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_FSTREAM_H */
