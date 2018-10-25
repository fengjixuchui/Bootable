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

/** @file dap.h API related to the disk address packet structure. */

#ifndef BOOTABLE_CORE_DAP_H
#define BOOTABLE_CORE_DAP_H

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bootable_stream;

/** This structure is used by the
 * boot sector to determine where
 * to load the 2nd and 3rd stage
 * boot loaders. The acronym DAP
 * stands for disk address packet.
 * */

struct bootable_dap {
	/** Reserved for use by the boot sector. */
	bootable_uint8 reserved[2];
	/** The number of sectors occupied by the boot loader. */
	bootable_uint16 sector_count;
	/** The address to load the boot loader at. */
	bootable_uint16 address;
	/** The segment to load the boot loader at. */
	bootable_uint16 segment;
	/** The sector that contains the boot loader. */
	bootable_uint64 sector;
};

/** Read the disk address packet from a stream.
 * @param dap A disk address packet structure.
 * @param stream The stream to read the data from.
 * @returns Zero on success, an error code on failure.
 * */

int bootable_dap_read(struct bootable_dap *dap, struct bootable_stream *stream);

/** Write the disk address packet to a stream.
 * @param dap A disk address packet structure.
 * @param stream The stream to wrote the data to.
 * @returns Zero on success, an error code on failure.
 * */

int bootable_dap_write(const struct bootable_dap *dap, struct bootable_stream *stream);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_CORE_DAP_H */
