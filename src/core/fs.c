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

#include <bootable/core/fs.h>
#include <bootable/core/file.h>
#include <bootable/core/path.h>
#include <bootable/core/error.h>
#include <bootable/core/string.h>

#include "misc.h"

#ifndef NULL
#define NULL ((void *) 0x00)
#endif

static bootable_uint64 bootable_file_size(const struct bootable_file *file) {
	return 16 + file->name_size + file->data_size;
}

static bootable_uint64 bootable_dir_size(const struct bootable_dir *dir) {

	bootable_uint64 size = 24;

	for (bootable_uint64 i = 0; i < dir->subdir_count; i++)
		size += bootable_dir_size(&dir->subdirs[i]);

	for (bootable_uint64 i = 0; i < dir->file_count; i++)
		size += bootable_file_size(&dir->files[i]);

	return size;
}

static bootable_uint64 bootable_fs_size(const struct bootable_fs *fs) {

	return 16 + bootable_dir_size(&fs->root);
}

void bootable_fs_init(struct bootable_fs *fs) {
	fs->signature = BOOTABLE_SIGNATURE;
	fs->size = 0;
	bootable_dir_init(&fs->root);
}

void bootable_fs_free(struct bootable_fs *fs) {
	bootable_dir_free(&fs->root);
}

int bootable_fs_export(struct bootable_fs *fs, struct bootable_stream *out) {

	int err;

	fs->size = bootable_fs_size(fs);

	err = encode_uint64(fs->signature, out);
	if (err != 0)
		return err;

	err = encode_uint64(fs->size, out);
	if (err != 0)
		return err;

	err = bootable_dir_export(&fs->root, out);
	if (err != 0)
		return err;

	return 0;
}

int bootable_fs_import(struct bootable_fs *fs, struct bootable_stream *in) {

	int err;

	err = decode_uint64(&fs->signature, in);
	if (err != 0)
		return err;

	if (fs->signature != BOOTABLE_SIGNATURE)
		return BOOTABLE_EINVAL;

	err = decode_uint64(&fs->size, in);
	if (err != 0)
		return err;

	err = bootable_dir_import(&fs->root, in);
	if (err != 0)
		return err;

	return 0;
}

int bootable_fs_make_dir(struct bootable_fs *fs, const char *path_str) {

	int err;
	const char *name;
	unsigned int name_count;
	unsigned int subdir_count;
	unsigned int i;
	unsigned int j;
	struct bootable_path path;
	struct bootable_dir *parent_dir;
	struct bootable_dir *subdir;

	bootable_path_init(&path);

	err = bootable_path_parse(&path, path_str);
	if (err != 0) {
		bootable_path_free(&path);
		return err;
	}

	err = bootable_path_normalize(&path);
	if (err != 0) {
		bootable_path_free(&path);
		return err;
	}

	parent_dir = &fs->root;

	name_count = bootable_path_get_name_count(&path);

	if (name_count == 0) {
		bootable_path_free(&path);
		return BOOTABLE_EINVAL;
	}

	for (i = 0; i < (name_count - 1); i++) {

		name = bootable_path_get_name(&path, i);
		if (name == NULL) {
			bootable_path_free(&path);
			return BOOTABLE_EFAULT;
		}

		subdir_count = parent_dir->subdir_count;

		for (j = 0; j < subdir_count; j++) {
			subdir = &parent_dir->subdirs[j];
			if (subdir == NULL) {
				continue;
			} else if (bootable_strcmp(subdir->name, name) == 0) {
				parent_dir = subdir;
				break;
			}
		}

		if (j >= subdir_count) {
			/* not found */
			bootable_path_free(&path);
			return BOOTABLE_ENOENT;
		}
	}

	if (i != (name_count - 1)) {
		bootable_path_free(&path);
		return BOOTABLE_ENOENT;
	}

	err = bootable_dir_add_subdir(parent_dir, path.name_array[i].data);
	if (err != 0) {
		bootable_path_free(&path);
		return err;
	}

	bootable_path_free(&path);

	return 0;
}

int bootable_fs_make_file(struct bootable_fs *fs, const char *path_str) {

	int err;
	const char *name;
	unsigned int name_count;
	unsigned int subdir_count;
	unsigned int i;
	unsigned int j;
	struct bootable_path path;
	struct bootable_dir *parent_dir;
	struct bootable_dir *subdir;

	bootable_path_init(&path);

	err = bootable_path_parse(&path, path_str);
	if (err != 0) {
		bootable_path_free(&path);
		return err;
	}

	err = bootable_path_normalize(&path);
	if (err != 0) {
		bootable_path_free(&path);
		return err;
	}

	parent_dir = &fs->root;

	name_count = bootable_path_get_name_count(&path);

	if (name_count == 0) {
		bootable_path_free(&path);
		return BOOTABLE_EINVAL;
	}

	for (i = 0; i < (name_count - 1); i++) {

		name = bootable_path_get_name(&path, i);
		if (name == NULL) {
			bootable_path_free(&path);
			return BOOTABLE_EINVAL;
		}

		subdir_count = parent_dir->subdir_count;

		for (j = 0; j < subdir_count; j++) {
			subdir = &parent_dir->subdirs[j];
			if (subdir == NULL) {
				continue;
			} else if (bootable_strcmp(subdir->name, name) == 0) {
				parent_dir = subdir;
				break;
			}
		}

		if (j >= subdir_count) {
			/* not found */
			bootable_path_free(&path);
			return BOOTABLE_ENOENT;
		}
	}

	if (i != (name_count - 1)) {
		bootable_path_free(&path);
		return BOOTABLE_ENOENT;
	}

	err = bootable_dir_add_file(parent_dir, path.name_array[i].data);
	if (err != 0) {
		bootable_path_free(&path);
		return err;
	}

	bootable_path_free(&path);

	return 0;
}

struct bootable_dir *bootable_fs_open_dir(struct bootable_fs *fs, const char *path_string) {

	int err;
	unsigned int i;
	unsigned int j;
	const char *name;
	unsigned int name_count;
	unsigned int subdir_count;
	struct bootable_path path;
	struct bootable_dir *parent_dir;

	bootable_path_init(&path);

	err = bootable_path_parse(&path, path_string);
	if (err != 0) {
		bootable_path_free(&path);
		return NULL;
	}

	err = bootable_path_normalize(&path);
	if (err != 0) {
		bootable_path_free(&path);
		return NULL;
	}

	parent_dir = &fs->root;

	name_count = bootable_path_get_name_count(&path);

	for (i = 0; i < name_count; i++) {
		name = bootable_path_get_name(&path, i);
		if (name == NULL) {
			bootable_path_free(&path);
			return NULL;
		}

		subdir_count = parent_dir->subdir_count;
		for (j = 0; j < subdir_count; j++) {
			if (bootable_strcmp(parent_dir->subdirs[j].name, name) == 0) {
				parent_dir = &parent_dir->subdirs[j];
				break;
			}
		}

		if (j >= subdir_count) {
			bootable_path_free(&path);
			return NULL;
		}
	}

	bootable_path_free(&path);

	return parent_dir;
}

struct bootable_file *bootable_fs_open_file(struct bootable_fs *fs, const char *path_string) {

	int err;
	unsigned int i;
	unsigned int j;
	const char *name;
	unsigned int subdir_count;
	unsigned int name_count;
	struct bootable_path path;
	struct bootable_dir *parent_dir;

	bootable_path_init(&path);

	err = bootable_path_parse(&path, path_string);
	if (err != 0) {
		bootable_path_free(&path);
		return NULL;
	}

	err = bootable_path_normalize(&path);
	if (err != 0) {
		bootable_path_free(&path);
		return NULL;
	}

	parent_dir = &fs->root;

	name_count = bootable_path_get_name_count(&path);

	if (name_count == 0) {
		/* there must be at least one
		 * entry name in the path */
		bootable_path_free(&path);
		return NULL;
	}

	for (i = 0; i < (name_count - 1); i++) {
		name = bootable_path_get_name(&path, i);
		if (name == NULL) {
			bootable_path_free(&path);
			return NULL;
		}

		subdir_count = parent_dir->subdir_count;
		for (j = 0; j < subdir_count; j++) {
			if (bootable_strcmp(parent_dir->subdirs[j].name, name) == 0) {
				parent_dir = &parent_dir->subdirs[j];
				break;
			}
		}

		if (j >= subdir_count) {
			bootable_path_free(&path);
			return NULL;
		}
	}

	name = bootable_path_get_name(&path, i);
	if (name == NULL) {
		/* This shouldn't happen, so
		 * this check is a precaution */
		bootable_path_free(&path);
		return NULL;
	}

	/* 'name' is now the basename of the file. */

	for (j = 0; j < parent_dir->file_count; j++) {
		if (bootable_strcmp(parent_dir->files[j].name, name) == 0) {
			bootable_path_free(&path);
			return &parent_dir->files[j];
		}
	}

	/* file not found */

	bootable_path_free(&path);

	return NULL;
}
