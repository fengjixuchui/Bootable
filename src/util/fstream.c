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

#include "fstream.h"

#include <bootable/core/error.h>

#include <limits.h>
#include <stdlib.h>

static int fstream_set_pos(void *file_ptr, bootable_uint64 pos_ptr) {

	if (pos_ptr > LONG_MAX)
		return BOOTABLE_EINVAL;

	if (fseek((FILE *) file_ptr, pos_ptr, SEEK_SET) != 0)
		return BOOTABLE_EIO;

	return 0;
}

static int fstream_get_pos(void *file_ptr, bootable_uint64 *pos_ptr) {

	long int pos;

	pos = ftell((FILE *) file_ptr);
	if (pos == -1L)
		return BOOTABLE_EIO;

	*pos_ptr = pos;

	return 0;
}

static int fstream_get_size(void *file_ptr, bootable_uint64 *size_ptr) {

	FILE *file = (FILE *) file_ptr;
	if (file == NULL)
		return BOOTABLE_EFAULT;

	long int original_pos = ftell(file);
	if (original_pos == -1L)
		/* TODO : more specific error code */
		return BOOTABLE_EINVAL;

	if (fseek(file, 0, SEEK_END) != 0)
		/* TODO : more specific error code */
		return BOOTABLE_EINVAL;

	long int size = ftell(file);
	if (size == -1L)
		/* TODO : more specific error code */
		return BOOTABLE_EINVAL;

	if (fseek(file, original_pos, SEEK_SET) != 0)
		/* TODO : more specific error code */
		return BOOTABLE_EINVAL;

	if (size_ptr != NULL)
		*size_ptr = (bootable_uint64) size;

	return 0;
}

static int fstream_write(void *file_ptr, const void *buf, bootable_uint64 buf_size) {
	if (fwrite(buf, 1, buf_size, (FILE *) file_ptr) != buf_size)
		return BOOTABLE_EIO;
	else
		return 0;
}

static int fstream_read(void *file_ptr, void *buf, bootable_uint64 buf_size) {

	if (file_ptr == NULL)
		return BOOTABLE_EFAULT;

	if (fread(buf, 1, buf_size, (FILE *) file_ptr) != buf_size)
		return BOOTABLE_EIO;
	else
		return 0;
}

void bootable_fstream_init(struct bootable_fstream *fstream) {
	bootable_stream_init(&fstream->base);
	fstream->base.data = NULL;
	fstream->base.set_pos = fstream_set_pos;
	fstream->base.get_pos = fstream_get_pos;
	fstream->base.get_size = fstream_get_size;
	fstream->base.write = fstream_write;
	fstream->base.read = fstream_read;
	fstream->file = NULL;
}

void bootable_fstream_done(struct bootable_fstream *fstream) {
	if (fstream->file != NULL) {
		fclose(fstream->file);
		fstream->file = NULL;
		fstream->base.data = NULL;
	}
}

int bootable_fstream_resize(struct bootable_fstream *fstream,
                          long int size) {

	if (size < 1)
		return BOOTABLE_EINVAL;

	int err = bootable_stream_set_pos(&fstream->base, size - 1);
	if (err != 0)
		return err;

	err = bootable_stream_write(&fstream->base, "\x00", 1);
	if (err != 0)
		return err;

	return 0;
}

int bootable_fstream_open(struct bootable_fstream *fstream,
                        const char *path,
                        const char *mode) {

	FILE *file = fopen(path, mode);
	if (file == NULL) {
		/* TODO : more specific error code */
		return BOOTABLE_ENOENT;
	}

	if (fstream->file != NULL)
		fclose(fstream->file);

	fstream->file = file;
	fstream->base.data = file;

	return 0;
}

int bootable_fstream_zero(struct bootable_fstream *fstream,
                        unsigned long int count) {

	unsigned long int buf_size = 4096;

	unsigned char *buf = calloc(1, buf_size);

	unsigned long int read_count = 0;

	while ((read_count + buf_size) < count) {

		int err = bootable_stream_write(&fstream->base, buf, buf_size);
		if (err != 0) {
			free(buf);
			return err;
		}

		read_count += buf_size;
	}

	int err = bootable_stream_write(&fstream->base, buf, count - read_count);
	if (err != 0) {
		free(buf);
		return err;
	}

	free(buf);

	return 0;
}
