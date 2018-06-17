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

#include <bootable/core/path.h>
#include <bootable/core/error.h>
#include <bootable/core/memory.h>
#include <bootable/core/string.h>

static int is_separator(char c) {
	if ((c == '/') || (c == '\\'))
		return 1;
	else
		return 0;
}

void bootable_path_init(struct bootable_path *path) {
	path->name_array = bootable_null;
	path->name_count = 0;
}

void bootable_path_free(struct bootable_path *path) {

	bootable_uint64 i;

	for (i = 0; i < path->name_count; i++)
		bootable_free(path->name_array[i].data);

	bootable_free(path->name_array);

	path->name_array = bootable_null;
	path->name_count = 0;
}

const char * bootable_path_get_name(const struct bootable_path *path,
                                  bootable_uint64 index) {

	if (index >= path->name_count)
		return bootable_null;

	return path->name_array[index].data;
}

bootable_uint64 bootable_path_get_name_count(const struct bootable_path *path) {

	return path->name_count;
}

int bootable_path_normalize(struct bootable_path *path) {

	bootable_uint64 i;
	bootable_uint64 j;

	i = 0;

	while (i < path->name_count) {

		if (path->name_array[i].data == bootable_null) {

			return BOOTABLE_EFAULT;

		} else if (bootable_strcmp(path->name_array[i].data, ".") == 0) {

			bootable_free(path->name_array[i].data);

			for (j = i + 1; j < path->name_count; j++) {
				path->name_array[j - 1] = path->name_array[j];
			}

			path->name_count--;

		} else if (bootable_strcmp(path->name_array[i].data, "..") == 0) {

			bootable_free(path->name_array[i].data);

			if (i == 0) {
				path->name_count--;
				continue;
			}

			i--;

			bootable_free(path->name_array[i].data);

			for (j = i + 2; j < path->name_count; j++)
				path->name_array[j - 2] = path->name_array[j];

			path->name_count -= 2;

		} else {
			i++;
			continue;
		}
	}

	return 0;
}

int bootable_path_parse(struct bootable_path *path,
                      const char *path_str) {

	int err;
	bootable_uint64 i;
	char *tmp;
	char *tmp2;
	bootable_uint64 tmp_size;
	bootable_uint64 tmp_res;

	tmp = bootable_null;
	tmp_size = 0;
	tmp_res = 0;

	for (i = 0; path_str[i]; i++) {
		if (is_separator(path_str[i])) {

			if (tmp_size == 0)
				continue;

			err = bootable_path_push_child(path, tmp);
			if (err != 0) {
				bootable_free(tmp);
				return err;
			}

			tmp_size = 0;

		} else {

			if ((tmp_size + 1) >= tmp_res) {
				tmp_res += 64;
				tmp2 = bootable_realloc(tmp, tmp_res);
				if (tmp2 == bootable_null) {
					bootable_free(tmp);
					return BOOTABLE_ENOMEM;
				}
				tmp = tmp2;
			}

			tmp[tmp_size++] = path_str[i];
			tmp[tmp_size] = 0;
		}
	}

	if (tmp_size > 0) {
		err = bootable_path_push_child(path, tmp);
		if (err != 0) {
			bootable_free(tmp);
			return err;
		}
	}

	bootable_free(tmp);

	return 0;
}

int bootable_path_push_child(struct bootable_path *path,
                           const char *name) {

	char *tmp_name;
	bootable_uint64 name_size;
	struct bootable_path_name *name_array;
	bootable_uint64 name_array_size;

	name_array = path->name_array;

	name_array_size = path->name_count + 1;
	name_array_size *= sizeof(path->name_array[0]);

	name_array = bootable_realloc(name_array, name_array_size);
	if (name_array == bootable_null)
		return BOOTABLE_ENOMEM;

	path->name_array = name_array;

	name_size = bootable_strlen(name);

	tmp_name = bootable_malloc(name_size + 1);
	if (tmp_name == bootable_null)
		return BOOTABLE_ENOMEM;

	bootable_memcpy(tmp_name, name, name_size);

	tmp_name[name_size] = 0;

	path->name_array[path->name_count].data = tmp_name;
	path->name_array[path->name_count].size = name_size;

	path->name_count++;

	return 0;
}
