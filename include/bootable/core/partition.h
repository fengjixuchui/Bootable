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

/** @file partition.h API related to partion
 * data structures and functions.
 * */

#ifndef BOOTABLE_PARTITION_H
#define BOOTABLE_PARTITION_H

#include <bootable/core/stream.h>
#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** This structure represents a
 * partition contained within a
 * disk recognized by Pure64.
 * */

struct bootable_partition {
	/** The stream of the partition. */
	struct bootable_stream stream;
	/** The stream of the disk that
	 * contains the partition. */
	struct bootable_stream *disk;
	/** The beginning of the partition,
	 * in bytes.  */
	bootable_uint64 offset;
	/** The number of bytes that
	 * are occupied by the partition. */
	bootable_uint64 size;
	/** The position of the next read
	 * or write operation within the
	 * partition. */
	bootable_uint64 position;
};

/** Initializes a Pure64 partition.
 * @param partition The partition structure
 * to initialize.
 * */

void bootable_partition_init(struct bootable_partition *partition);

/** Release memory allocated by the
 * partition structure.
 * @param partition The partition to
 * release the memory for.
 * */

void bootable_partition_done(struct bootable_partition *partition);

/** Assign the disk stream to use for the
 * partition. The start and offset of
 * the partition should also be set,
 * so that the read and write functions
 * function within the bounds of the partition.
 * @param partition The partition structure
 * to assign the disk stream to.
 * @param disk The disk stream to assign
 * to the partition. This would typically
 * be a disk but it does not have to be.
 * */

void bootable_partition_set_disk(struct bootable_partition *partition,
                               struct bootable_stream *disk);

/** Set the number of bytes contained by the partition.
 * @param partition The partition to set the size of.
 * @param size The size, in bytes, of the partition.
 * This can be any value from zero to the size of the
 * medium containing the partition.
 * */

void bootable_partition_set_size(struct bootable_partition *partition,
                               bootable_uint64 size);

/** Set the starting point of the partition, in terms
 * of bytes. After using this function, read and write
 * operations starting at zero will actually begin at
 * this location.
 * @param partition The partition to set the offset for.
 * @param offset The offset of the partition. This may
 * be any value from zero to the size of the partition.
 * */

void bootable_partition_set_offset(struct bootable_partition *partition,
                                 bootable_uint64 offset);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_PARTITION_H */
