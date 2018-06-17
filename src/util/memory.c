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

#include <bootable/core/memory.h>

#include <stdlib.h>

void *bootable_malloc(bootable_uint64 size) {
	return malloc(size);
}

void *bootable_realloc(void *addr, bootable_uint64 size) {
	return realloc(addr, size);
}

void bootable_free(void *addr) {
	free(addr);
}
