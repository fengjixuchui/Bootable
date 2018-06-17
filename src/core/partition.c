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

#include <bootable/core/partition.h>

#include <bootable/core/error.h>

#ifndef NULL
#define NULL ((void *) 0x00)
#endif

static int partition_read(void *partition_ptr, void *buf, bootable_uint64 buf_size) {

	struct bootable_partition *partition = (struct bootable_partition *) partition_ptr;
	if ((partition == NULL) || (partition->disk == NULL))
		return BOOTABLE_EFAULT;

	int err = bootable_stream_set_pos(partition->disk, partition->offset + partition->position);
	if (err != 0)
		return err;

	partition->position += buf_size;

	return bootable_stream_read(partition->disk, buf, buf_size);
}

static int partition_write(void *partition_ptr, const void *buf, bootable_uint64 buf_size) {

	struct bootable_partition *partition = (struct bootable_partition *) partition_ptr;
	if ((partition == NULL) || (partition->disk == NULL))
		return BOOTABLE_EFAULT;

	int err = bootable_stream_set_pos(partition->disk, partition->offset + partition->position);
	if (err != 0)
		return err;

	partition->position += buf_size;

	return bootable_stream_write(partition->disk, buf, buf_size);
}

static int partition_set_pos(void *partition_ptr, bootable_uint64 pos) {

	struct bootable_partition *partition = (struct bootable_partition *) partition_ptr;
	if ((partition == NULL)|| (partition->disk == NULL))
		return BOOTABLE_EFAULT;

	if (pos > partition->size)
		return BOOTABLE_EINVAL;

	partition->position = pos;

	return 0;
}

static int partition_get_pos(void *partition_ptr, bootable_uint64 *pos) {

	struct bootable_partition *partition = (struct bootable_partition *) partition_ptr;
	if (partition == NULL)
		return BOOTABLE_EFAULT;

	if (pos != NULL)
		*pos = partition->position;

	return 0;
}

static int partition_get_size(void *partition_ptr, bootable_uint64 *size) {

	struct bootable_partition *partition = (struct bootable_partition *) partition_ptr;
	if (partition == NULL)
		return BOOTABLE_EFAULT;

	if (size != NULL)
		*size = partition->size;

	return 0;
}

void bootable_partition_init(struct bootable_partition *partition) {
	bootable_stream_init(&partition->stream);
	partition->stream.data = partition;
	partition->stream.read = partition_read;
	partition->stream.write = partition_write;
	partition->stream.set_pos = partition_set_pos;
	partition->stream.get_pos = partition_get_pos;
	partition->stream.get_size = partition_get_size;
	partition->disk = NULL;
	partition->offset = 0;
	partition->size = 0;
	partition->position = 0;
}

void bootable_partition_done(struct bootable_partition *partition) {
	/* Currently nothing to do here. */
	(void) partition;
}

void bootable_partition_set_disk(struct bootable_partition *partition,
                               struct bootable_stream *disk) {

	if (disk != NULL)
		partition->disk = disk;
}

void bootable_partition_set_size(struct bootable_partition *partition,
                               bootable_uint64 size) {

	partition->size = size;
}

void bootable_partition_set_offset(struct bootable_partition *partition,
                                 bootable_uint64 offset) {

	partition->offset = offset;
}
