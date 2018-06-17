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

/** @file uuid.h UUID related functions and structures. */

#ifndef BOOTABLE_UUID_H
#define BOOTABLE_UUID_H

#ifndef BOOTABLE_UUID_DISK

/** The UUID of the disk formatted
 * with a GUID partition table.
 * */

#define BOOTABLE_UUID_DISK "dc301a8c-1632-45a6-962c-aa4f1cc35943"

#endif

#ifndef BOOTABLE_UUID_STAGE_TWO

/** The UUID for the stage
 * two boot loader partition.
 * */

#define BOOTABLE_UUID_STAGE_TWO "daa1ab4e-7a2c-4404-8208-61a12c660382"

#endif

#ifndef BOOTABLE_UUID_STAGE_THREE

/** The UUID of the stage
 * three partition.
 * */

#define BOOTABLE_UUID_STAGE_THREE "32cfd7f2-0e0a-4908-8d3b-16d7fb3a3c57"

#endif

#ifndef BOOTABLE_UUID_FILE_SYSTEM

/** The UUID for Pure64's file system.
 * */

#define BOOTABLE_UUID_FILE_SYSTEM "f7439905-43da-4df0-b863-1f456e008b58"

#endif

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Universally unique identifer.
 * Used to identify various parts
 * of the Pure64 partition table.
 * */

struct bootable_uuid {
	/** The byte oriented layout
	 * of the identifier. */
	bootable_uint8 bytes[16];
};

/** Zero the value of an identifier.
 * This may be desired for initialization
 * purposes. A UUID of all zeros indicates
 * that it was not initialized properly.
 * @param uuid A UUID structure.
 * */

void bootable_uuid_zero(struct bootable_uuid *uuid);

/** Copies a UUID.
 * @param dst The destination of the UUID.
 * @param src The UUID to copy over to the
 * destination.
 * */

void bootable_uuid_copy(struct bootable_uuid *dst,
                      const struct bootable_uuid *src);

/** Compares two UUIDs.
 * @param a The first UUID
 * @param b The second UUID
 * @returns Zero, if the UUIDs are equal.
 * One, if @p a is greater than @p b.
 * Negative one, if @p is less than @p b.
 * */

int bootable_uuid_cmp(const struct bootable_uuid *a,
                    const struct bootable_uuid *b);

/** Parse a UUID string.
 * @param uuid A UUID structure.
 * @param uuid_str A UUID string.
 * The first three numbers should be
 * little endian and the last two should
 * be big endian. It does not matter if
 * the hex characters are upper or lower case.
 * @returns Zero on success, @ref BOOTABLE_EINVAL
 * if the string was formatted incorrectly, and
 * @ref BOOTABLE_EFAULT if an address was NULL.
 * */

int bootable_uuid_parse(struct bootable_uuid *uuid, const char *uuid_str);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_UUID_H */
