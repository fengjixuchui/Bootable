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

/** @file mbr.h API related to the master boot record. */

#ifndef BOOTABLE_MBR_H
#define BOOTABLE_MBR_H

#include <bootable/core/dap.h>
#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bootable_stream;

/** The master boot record.
 * Also contains information
 * used to load the partition
 * containing the 2nd stage
 * boot loader.
 * */

struct bootable_mbr {
	/** The assembly code used
	 * to load the next boot loader. */
	bootable_uint8 code[476];
	/** The disk address packet for
	 * the second stage boot loader. */
	struct bootable_dap st2dap;
	/** The disk address packet for
	 * the third stage boot loader. */
	struct bootable_dap st3dap;
	/** The boot signature. */
	bootable_uint16 boot_signature;
};

/** Zero out the MBR structure.
 * @param mbr An MBR structure.
 * */

void bootable_mbr_zero(struct bootable_mbr *mbr);

/** Check if the MBR is valid.
 * This function checks if the MBR
 * has a boot signature.
 * @param mbr The MBR structure to check.
 * @returns Zero if the MBR is valid.
 * If it is not, then @ref BOOTABLE_EINVAL
 * is returned.
 * */

int bootable_mbr_check(const struct bootable_mbr *mbr);

/** Read an MBR sector from a stream.
 * The MBR is read from the beginning of the stream.
 * @param mbr The MBR structure to put the data into.
 * @param stream The stream to read the MBR from.
 * @returns Zero on success, an error code on failure.
 * */

int bootable_mbr_read(struct bootable_mbr *mbr, struct bootable_stream *stream);

/** Write an MBR sector to a stream.
 * The MBR is written to the beginning of the stream.
 * @param mbr The MBR structure to get the data from.
 * @param stream The stream to write the MBR to.
 * @returns Zero on success, an error code on failure.
 * */

int bootable_mbr_write(const struct bootable_mbr *mbr, struct bootable_stream *stream);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_MBR_H */
