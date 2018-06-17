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

#include <bootable/core/file.h>
#include <bootable/core/error.h>
#include <bootable/core/memory.h>
#include <bootable/core/stream.h>
#include <bootable/core/string.h>

#include "misc.h"

void bootable_file_init(struct bootable_file *file) {
	file->name_size = 0;
	file->data_size = 0;
	file->name = bootable_null;
	file->data = bootable_null;
}

void bootable_file_free(struct bootable_file *file) {
	bootable_free(file->name);
	bootable_free(file->data);
	file->name = bootable_null;
	file->data = bootable_null;
}

int bootable_file_export(struct bootable_file *file, struct bootable_stream *out) {

	int err;

	err = encode_uint64(file->name_size, out);
	if (err != 0)
		return err;

	err = encode_uint64(file->data_size, out);
	if (err != 0)
		return err;

	err = bootable_stream_write(out, file->name, file->name_size);
	if (err != 0)
		return err;

	err = bootable_stream_write(out, file->data, file->data_size);
	if (err != 0)
		return err;

	return 0;
}

int bootable_file_import(struct bootable_file *file, struct bootable_stream *in) {

	int err;

	err = decode_uint64(&file->name_size, in);
	if (err != 0)
		return err;

	err = decode_uint64(&file->data_size, in);
	if (err != 0)
		return err;

	file->name = bootable_malloc(file->name_size + 1);
	file->data = bootable_malloc(file->data_size);
	if ((file->name == bootable_null) || (file->data == bootable_null)) {
		bootable_free(file->name);
		bootable_free(file->data);
		return BOOTABLE_ENOMEM;
	}

	err = bootable_stream_read(in, file->name, file->name_size);
	if (err != 0)
		return err;

	file->name[file->name_size] = 0;

	err = bootable_stream_read(in, file->data, file->data_size);
	if (err != 0)
		return err;

	return 0;
}

int bootable_file_set_name(struct bootable_file *file, const char *name) {

	char *tmp_name;
	unsigned long int name_size;

	name_size = bootable_strlen(name);

	tmp_name = bootable_malloc(name_size + 1);
	if (tmp_name == bootable_null) {
		return BOOTABLE_ENOMEM;
	}

	bootable_memcpy(tmp_name, name, name_size);

	tmp_name[name_size] = 0;

	bootable_free(file->name);

	file->name = tmp_name;
	file->name_size = name_size;

	return 0;
}
