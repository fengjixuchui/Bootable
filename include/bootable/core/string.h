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

/** @file string.h API related to Pure64's string functions. */

#ifndef BOOTABLE_STRING_H
#define BOOTABLE_STRING_H

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Set a range of memory to a certain value.
 * @param dst The destination address.
 * @param value The value to set each byte in the
 * memory section. This value is truncated to an 8-bit,
 * unsigned value.
 * @param size The number of bytes to set in the
 * memory section.
 * */

void bootable_memset(void *dst, int value, bootable_uint64 size);

/** Copy a range of memory from one location
 * to the other. This function does not check
 * for overlapping ranges.
 * @param dst The destination memory section.
 * @param src The memory section to copy.
 * @param size The number of bytes to copy.
 * */

void bootable_memcpy(void *dst, const void *src, bootable_uint64 size);

/** Compare two blocks of memory.
 * @param a The first block
 * @param b The second block
 * @param size The number of bytes
 * in each block of memory.
 * @returns Zero if the blocks are
 * equal, one if block @p a is greater
 * than block @p b, and negative one
 * if block @p a is less than block @p b.
 * */

int bootable_memcmp(const void *a, const void *b, bootable_uint64 size);

/** Calculate the length of a null-terminated string.
 * @param str The string to calculate the
 * length of. This must be null-terminated.
 * @returns The size of the string, not including
 * the null-terminator, in bytes.
 * */

bootable_uint64 bootable_strlen(const char *str);

/** Compares two strings.
 * @param a The first string to compare.
 * @param b The second string to compare.
 * @returns If a byte at @p a is greater the
 * the byte at @p b, then one is returned.
 * If they are equal then zero is returned.
 * Otherwise, a negative one is returned.
 * */

int bootable_strcmp(const char *a, const char *b);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_STRING_H */
