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

#include <bootable/core/string.h>

void bootable_memset(void *dst, int value, bootable_uint64 size) {

	unsigned char *dst8;
	bootable_uint64 i;

	dst8 = (unsigned char *) dst;

	for (i = 0; i < size; i++) {
		dst8[i] = (unsigned char) value;
	}
}

void bootable_memcpy(void *dst, const void *src, bootable_uint64 size) {

	unsigned char *dst8;
	const unsigned char *src8;
	bootable_uint64 i;

	dst8 = (unsigned char *) dst;
	src8 = (const unsigned char *) src;

	for (i = 0; i < size; i++) {
		dst8[i] = src8[i];
	}
}

int bootable_memcmp(const void *a, const void *b, bootable_uint64 size) {

	const unsigned char *a8 = (const unsigned char *) a;
	const unsigned char *b8 = (const unsigned char *) b;

	for (bootable_uint64 i = 0; i < size; i++) {
		if (a8[i] < b8[i])
			return -1;
		else if (a8[i] > b8[i])
			return 1;
	}

	return 0;
}

bootable_uint64 bootable_strlen(const char *str) {

	bootable_uint64 i = 0;

	while (str[i] != 0)
		i++;

	return i;
}

int bootable_strcmp(const char *a, const char *b) {

	while (*a && *a == *b) {
		a++;
		b++;
	}

	if (*a > *b)
		return 1;
	else if (*a < *b)
		return -1;
	else
		return 0;
}
