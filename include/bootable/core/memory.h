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

/** @file memory.h API related to memory allocation functions. */

#ifndef BOOTABLE_MEMORY_H
#define BOOTABLE_MEMORY_H

#include <bootable/core/types.h>

/** Represents an invalid memory
 * reference.
 * */

#ifdef __cplusplus
extern "C" {
#endif

/** Allocates memory of a specified size.
 * @param size The number of bytes to allocate.
 * @returns The addresss of the memory on success,
 * zero if there is no more memory available.
 * */

void *bootable_malloc(bootable_uint64 size);

/** Resizes an existing memory block. If the
 * address passed to this function is zero, then
 * a new memory block is allocated.
 * @param addr The existing memory address, if applicable.
 * @param size The new size that the existing memory block
 * should fit. If a new memory block is being allocated,
 * then this should be the initial size of the memory block.
 * @returns The address of the new memory block, if successful.
 * Otherwise, zero is returned.
 * */

void *bootable_realloc(void *addr, bootable_uint64 size);

/** Releases a memory block previously allocated.
 * @param addr The address of the memory block that
 * was allocated with @ref bootable_malloc or @ref bootable_realloc.
 * This may also be zero, in which case this function does nothing.
 * */

void bootable_free(void *addr);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_MEMORY_H */
