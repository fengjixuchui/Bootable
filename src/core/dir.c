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

#include <bootable/core/dir.h>
#include <bootable/core/error.h>
#include <bootable/core/file.h>
#include <bootable/core/memory.h>
#include <bootable/core/path.h>
#include <bootable/core/stream.h>
#include <bootable/core/string.h>

#include "misc.h"

void bootable_dir_init(struct bootable_dir *dir) {
	dir->name_size = 0;
	dir->subdir_count = 0;
	dir->file_count = 0;
	dir->name = bootable_null;
	dir->subdirs = bootable_null;
	dir->files = bootable_null;
}

void bootable_dir_free(struct bootable_dir *dir) {

	bootable_free(dir->name);

	for (bootable_uint64 i = 0; i < dir->subdir_count; i++)
		bootable_dir_free(&dir->subdirs[i]);

	for (bootable_uint64 i = 0; i < dir->file_count; i++)
		bootable_file_free(&dir->files[i]);

	bootable_free(dir->subdirs);
	bootable_free(dir->files);

	dir->name = bootable_null;
	dir->subdirs = bootable_null;
	dir->files = bootable_null;
}

int bootable_dir_add_file(struct bootable_dir *dir, const char *name) {

	int err;
	struct bootable_file *files;
	bootable_uint64 files_size;

	if (bootable_dir_name_exists(dir, name))
		return BOOTABLE_EEXIST;

	files_size = dir->file_count + 1;
	files_size *= sizeof(dir->files[0]);

	files = dir->files;

	files = bootable_realloc(files, files_size);
	if (files == bootable_null) {
		return BOOTABLE_ENOMEM;
	}

	bootable_file_init(&files[dir->file_count]);

	err = bootable_file_set_name(&files[dir->file_count], name);
	if (err != 0) {
		bootable_file_free(&files[dir->file_count]);
		return err;
	}

	dir->files = files;
	dir->file_count++;

	return 0;
}

int bootable_dir_add_subdir(struct bootable_dir *dir, const char *name) {

	int err;
	struct bootable_dir *subdirs;
	bootable_uint64 subdirs_size;

	if (bootable_dir_name_exists(dir, name))
		return BOOTABLE_EEXIST;

	subdirs_size = dir->subdir_count + 1;
	subdirs_size *= sizeof(dir->subdirs[0]);

	subdirs = dir->subdirs;

	subdirs = bootable_realloc(subdirs, subdirs_size);
	if (subdirs == bootable_null) {
		return BOOTABLE_ENOMEM;
	}

	bootable_dir_init(&subdirs[dir->subdir_count]);

	err = bootable_dir_set_name(&subdirs[dir->subdir_count], name);
	if (err != 0) {
		bootable_dir_free(&subdirs[dir->subdir_count]);
		return err;
	}

	dir->subdirs = subdirs;
	dir->subdir_count++;

	return 0;
}

int bootable_dir_export(struct bootable_dir *dir, struct bootable_stream *out) {

	int err;

	err = encode_uint64(dir->name_size, out);
	if (err != 0)
		return err;

	err = encode_uint64(dir->subdir_count, out);
	if (err != 0)
		return err;

	err = encode_uint64(dir->file_count, out);
	if (err != 0)
		return err;

	err = bootable_stream_write(out, dir->name, dir->name_size);
	if (err != 0)
		return err;

	for (bootable_uint64 i = 0; i < dir->subdir_count; i++) {
		err = bootable_dir_export(&dir->subdirs[i], out);
		if (err != 0)
			return err;
	}

	for (bootable_uint64 i = 0; i < dir->file_count; i++) {
		err = bootable_file_export(&dir->files[i], out);
		if (err != 0)
			return err;
	}

	return 0;
}

int bootable_dir_import(struct bootable_dir *dir, struct bootable_stream *in) {

	int err;

	err = decode_uint64(&dir->name_size, in);
	if (err != 0)
		return err;

	err = decode_uint64(&dir->subdir_count, in);
	if (err != 0)
		return err;

	err = decode_uint64(&dir->file_count, in);
	if (err != 0)
		return err;

	dir->name = bootable_malloc(dir->name_size + 1);
	dir->subdirs = bootable_malloc(dir->subdir_count * sizeof(dir->subdirs[0]));
	dir->files = bootable_malloc(dir->file_count * sizeof(dir->files[0]));
	if ((dir->name == bootable_null)
	 || (dir->subdirs == bootable_null)
	 || (dir->files == bootable_null)) {
		bootable_free(dir->name);
		bootable_free(dir->subdirs);
		bootable_free(dir->files);
		return BOOTABLE_ENOMEM;
	}

	err = bootable_stream_read(in, dir->name, dir->name_size);
	if (err != 0)
		return err;

	dir->name[dir->name_size] = 0;

	for (bootable_uint64 i = 0; i < dir->subdir_count; i++)
		bootable_dir_init(&dir->subdirs[i]);

	for (bootable_uint64 i = 0; i < dir->file_count; i++)
		bootable_file_init(&dir->files[i]);

	for (bootable_uint64 i = 0; i < dir->subdir_count; i++) {
		err = bootable_dir_import(&dir->subdirs[i], in);
		if (err != 0)
			return err;
	}

	for (bootable_uint64 i = 0; i < dir->file_count; i++) {
		err = bootable_file_import(&dir->files[i], in);
		if (err != 0)
			return err;
	}

	return 0;
}

bootable_bool bootable_dir_name_exists(const struct bootable_dir *dir, const char *name) {

	bootable_uint64 i;

	for (i = 0; i < dir->file_count; i++) {
		if (bootable_strcmp(dir->files[i].name, name) == 0)
			return bootable_true;
	}

	for (i = 0; i < dir->subdir_count; i++) {
		if (bootable_strcmp(dir->subdirs[i].name, name) == 0)
			return bootable_true;
	}

	return bootable_false;
}

int bootable_dir_set_name(struct bootable_dir *dir, const char *name) {

	char *tmp_name;
	bootable_uint64 name_size;

	name_size = bootable_strlen(name);

	tmp_name = bootable_malloc(name_size + 1);
	if (tmp_name == bootable_null)
		return BOOTABLE_ENOMEM;

	bootable_memcpy(tmp_name, name, name_size);

	tmp_name[name_size] = 0;

	bootable_free(dir->name);

	dir->name = tmp_name;
	dir->name_size = name_size;

	return 0;
}
