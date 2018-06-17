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

#include <bootable/core/gpt.h>

#include <bootable/core/error.h>
#include <bootable/core/memory.h>
#include <bootable/core/stream.h>
#include <bootable/core/string.h>
#include <bootable/core/uuid.h>

#include "misc.h"

#ifndef NULL
#define NULL ((void *) 0x00)
#endif

#ifndef GPT_ENTRY_SIZE
#define GPT_ENTRY_SIZE 128
#endif

#ifndef GPT_ENTRY_COUNT
#define GPT_ENTRY_COUNT BOOTABLE_GPT_ENTRY_COUNT
#endif

#ifndef GPT_HEADER_SIZE
#define GPT_HEADER_SIZE 92
#endif

#ifndef GPT_INVALID_LBA
#define GPT_INVALID_LBA BOOTABLE_GPT_INVALID_LBA
#endif

static void init_header(struct bootable_gpt_header *header) {
	header->signature[0] = 'E';
	header->signature[1] = 'F';
	header->signature[2] = 'I';
	header->signature[3] = ' ';
	header->signature[4] = 'P';
	header->signature[5] = 'A';
	header->signature[6] = 'R';
	header->signature[7] = 'T';
	header->version = 0x010000;
	header->checksum = 0;
	header->reserved = 0;
	header->current_lba = GPT_INVALID_LBA;
	header->backup_lba = GPT_INVALID_LBA;
	header->first_usable_lba = GPT_INVALID_LBA;
	header->last_usable_lba = GPT_INVALID_LBA;
	bootable_uuid_zero(&header->disk_uuid);
	header->partition_entries_lba = GPT_INVALID_LBA;
	header->partition_entry_count = 0;
	header->partition_entry_size = GPT_ENTRY_SIZE;
	header->partition_entries_checksum = 0;
}

static bootable_uint32 crc32(const void *buf, bootable_uint64 buf_size) {

	bootable_uint64 i;
	bootable_uint64 j;
	bootable_uint32 byte;
	bootable_uint32 crc;
	bootable_uint32 mask;
	const bootable_uint8 *buf8;

	crc = 0xFFFFFFFF;

	buf8 = (const bootable_uint8 *) buf;

	for (i = 0; i < buf_size; i++) {
		byte = buf8[i];
		crc = crc ^ byte;
		for (j = 0; j < 8; j++) {
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
	}

	return ~crc;
}

static int encode_uint32_at(bootable_uint32 n, struct bootable_stream *stream, bootable_uint64 pos) {

	int err = bootable_stream_set_pos(stream, pos);
	if (err != 0)
		return err;

	err = encode_uint32(n, stream);
	if (err != 0)
		return err;

	return 0;
}

static int calculate_header_checksum(struct bootable_stream *stream, bootable_uint64 header_location) {

	bootable_uint32 checksum;

	/* start with header */

	int err = bootable_stream_set_pos(stream, header_location);
	if (err != 0)
		return err;

	unsigned char buf[GPT_HEADER_SIZE];

	err = bootable_stream_read(stream, buf, sizeof(buf));
	if (err != 0)
		return err;

	checksum = crc32(buf, sizeof(buf));

	err = bootable_stream_set_pos(stream, header_location + 16);
	if (err != 0)
		return err;

	err = encode_uint32(checksum, stream);
	if (err != 0)
		return err;

	return 0;
}

static int calculate_entries_checksum(struct bootable_stream *stream) {

	/* start with header */

	int err = bootable_stream_set_pos(stream, 1024);
	if (err != 0)
		return err;

	bootable_uint64 buf_size = GPT_ENTRY_SIZE * GPT_ENTRY_COUNT;

	void *buf = bootable_malloc(buf_size);

	err = bootable_stream_read(stream, buf, buf_size);
	if (err != 0) {
		bootable_free(buf);
		return err;
	}

	bootable_uint32 checksum = crc32(buf, buf_size);

	/* write the checksum to the primary header */

	err = encode_uint32_at(checksum, stream, 512 + 88);
	if (err != 0) {
		bootable_free(buf);
		return err;
	}

	/* find the backup header LBA from the primary header  */

	err = bootable_stream_set_pos(stream, 512 + 32);
	if (err != 0) {
		bootable_free(buf);
		return err;
	}

	bootable_uint64 backup_lba = 0;

	err = decode_uint64(&backup_lba, stream);
	if (err != 0) {
		bootable_free(buf);
		return err;
	}

	/* go to the backup entry array */

	bootable_uint64 backup_pos = backup_lba * 512;
	backup_pos -= GPT_ENTRY_COUNT * GPT_ENTRY_SIZE;

	err = bootable_stream_set_pos(stream, backup_pos);
	if (err != 0) {
		bootable_free(buf);
		return err;
	}

	err = bootable_stream_read(stream, buf, buf_size);
	if (err != 0) {
		bootable_free(buf);
		return err;
	}

	checksum = crc32(buf, buf_size);

	bootable_free(buf);

	/* encode the checksum for the backup entry array */

	err = bootable_stream_set_pos(stream, (backup_lba * 512) + 88);
	if (err != 0)
		return err;

	err = encode_uint32(checksum, stream);
	if (err != 0)
		return err;

	return 0;
}

static int calculate_checksums(struct bootable_stream *stream) {

	int err = calculate_entries_checksum(stream);
	if (err != 0) {
		return err;
	}

	err = calculate_header_checksum(stream, 512);
	if (err != 0)
		return err;

	err = bootable_stream_set_pos(stream, 512 + 32);
	if (err != 0)
		return err;

	bootable_uint64 backup_lba = 0;

	err = decode_uint64(&backup_lba, stream);
	if (err != 0)
		return err;

	err = calculate_header_checksum(stream, backup_lba * 512);
	if (err != 0)
		return err;

	return 0;
}

int bootable_gpt_header_import(struct bootable_gpt_header *header,
                             struct bootable_stream *stream) {

	/* signature */
	int err = bootable_stream_read(stream, header->signature, sizeof(header->signature));
	if (err != 0)
		return err;

	/* version */
	err = decode_uint32(&header->version, stream);
	if (err != 0)
		return err;

	/* header size */
	err = decode_uint32(&header->header_size, stream);
	if (err != 0)
		return err;

	/* crc32 of header (zero during calculation) */
	err = decode_uint32(&header->checksum, stream);
	if (err != 0)
		return err;

	/* reserved */
	err = decode_uint32(&header->reserved, stream);
	if (err != 0)
		return err;

	err = decode_uint64(&header->current_lba, stream);
	if (err != 0)
		return err;

	/* backup lba */
	err = decode_uint64(&header->backup_lba, stream);
	if (err != 0)
		return err;

	/* first usable lba */
	err = decode_uint64(&header->first_usable_lba, stream);
	if (err != 0)
		return err;

	/* last usable lba */
	err = decode_uint64(&header->last_usable_lba, stream);
	if (err != 0)
		return err;

	/* disk UUID */
	err = bootable_stream_read(stream, header->disk_uuid.bytes, 16);
	if (err != 0)
		return err;

	/* lba of partition entries */
	err = decode_uint64(&header->partition_entries_lba, stream);
	if (err != 0)
		return err;

	/* number of partition entries */
	err = decode_uint32(&header->partition_entry_count, stream);
	if (err != 0)
		return err;

	/* partition entry size */
	err = decode_uint32(&header->partition_entry_size, stream);
	if (err != 0)
		return err;

	/* crc32 of partition header array */
	err = decode_uint32(&header->partition_entries_checksum, stream);
	if (err != 0)
		return err;

	return 0;
}

int bootable_gpt_header_export(const struct bootable_gpt_header *header,
                             struct bootable_stream *stream) {

	/* signature */
	int err = bootable_stream_write(stream, "EFI PART", 8);
	if (err != 0)
		return err;

	/* version */
	err |= encode_uint32(0x010000, stream);
	if (err != 0)
		return err;

	/* header size */
	err |= encode_uint32(0x5c, stream);
	if (err != 0)
		return err;

	/* crc32 of header (zero during calculation) */
	err |= encode_uint32(0x00, stream);
	if (err != 0)
		return err;

	/* reserved */
	err |= encode_uint32(0x00, stream);
	if (err != 0)
		return err;

	/* current lba */
	err |= encode_uint64(header->current_lba, stream);
	if (err != 0)
		return err;

	/* backup lba */
	err |= encode_uint64(header->backup_lba, stream);
	if (err != 0)
		return err;

	/* first usable lba */
	err |= encode_uint64(header->first_usable_lba, stream);
	if (err != 0)
		return err;

	/* last usable lba */
	err |= encode_uint64(header->last_usable_lba, stream);
	if (err != 0)
		return err;

	/* disk UUID */
	err |= bootable_stream_write(stream, header->disk_uuid.bytes, 16);
	if (err != 0)
		return err;

	/* lba of partition entries */
	err |= encode_uint64(header->partition_entries_lba, stream);
	if (err != 0)
		return err;

	/* number of partition entries */
	err |= encode_uint32(header->partition_entry_count, stream);
	if (err != 0)
		return err;

	/* partition entry size */
	err |= encode_uint32(0x80, stream);
	if (err != 0)
		return err;

	/* crc32 of partition header array */
	err |= encode_uint32(0x00, stream);
	if (err != 0)
		return err;

	/* zero the rest of the sector */
	for (unsigned int i = 0; i < (512 - 92); i += 4) {
		err = bootable_stream_write(stream, "\x00\x00\x00\x00", 4);
		if (err != 0)
			return err;
	}

	return 0;
}

void bootable_gpt_entry_init(struct bootable_gpt_entry *entry) {
	bootable_uuid_zero(&entry->type_uuid);
	bootable_uuid_zero(&entry->entry_uuid);
	entry->first_lba = BOOTABLE_GPT_INVALID_LBA;
	entry->last_lba = BOOTABLE_GPT_INVALID_LBA;
	entry->attributes = 0;
	bootable_memset(entry->name, 0, sizeof(entry->name));
}

int bootable_gpt_entry_import(struct bootable_gpt_entry *entry,
                            struct bootable_stream *stream) {

	int err = bootable_stream_read(stream, entry->type_uuid.bytes, sizeof(entry->type_uuid.bytes));
	if (err != 0)
		return err;

	err = bootable_stream_read(stream, entry->entry_uuid.bytes, sizeof(entry->entry_uuid.bytes));
	if (err != 0)
		return err;

	err = decode_uint64(&entry->first_lba, stream);
	if (err != 0)
		return err;

	err = decode_uint64(&entry->last_lba, stream);
	if (err != 0)
		return err;

	err = decode_uint64(&entry->attributes, stream);
	if (err != 0)
		return err;

	err = bootable_stream_read(stream, entry->name, sizeof(entry->name));
	if (err != 0)
		return err;

	return 0;
}

int bootable_gpt_entry_export(const struct bootable_gpt_entry *entry,
                            struct bootable_stream *stream) {

	int err = bootable_stream_write(stream, entry->type_uuid.bytes, sizeof(entry->type_uuid.bytes));
	if (err != 0)
		return err;

	err = bootable_stream_write(stream, entry->entry_uuid.bytes, sizeof(entry->entry_uuid.bytes));
	if (err != 0)
		return err;

	err = encode_uint64(entry->first_lba, stream);
	if (err != 0)
		return err;

	err = encode_uint64(entry->last_lba, stream);
	if (err != 0)
		return err;

	err = encode_uint64(entry->attributes, stream);
	if (err != 0)
		return err;

	err = bootable_stream_write(stream, entry->name, sizeof(entry->name));
	if (err != 0)
		return err;

	return 0;
}

bootable_uint64 bootable_gpt_entry_get_offset(const struct bootable_gpt_entry *entry) {
	return entry->first_lba * 512;
}

bootable_uint64 bootable_gpt_entry_get_size(const struct bootable_gpt_entry *entry) {
	return ((entry->last_lba - entry->first_lba) + 1) * 512;
}

int bootable_gpt_entry_is_used(const struct bootable_gpt_entry *entry) {

	if ((entry->first_lba == GPT_INVALID_LBA)
	 || (entry->last_lba == GPT_INVALID_LBA))
		return 0;
	else
		return 1;
}

int bootable_gpt_entry_is_type(const struct bootable_gpt_entry *entry,
                             const char *type_uuid_str) {

	struct bootable_uuid type_uuid;

	if (bootable_uuid_parse(&type_uuid, type_uuid_str) != 0) {
		/* If the UUID is invalid, then
		 * this partition must not be of
		 * this type. */
		return 0;
	}

	if (bootable_uuid_cmp(&type_uuid, &entry->type_uuid) == 0) {
		/* The UUIDs match */
		return 1;
	}

	return 0;
}

void bootable_gpt_init(struct bootable_gpt *gpt) {
	init_header(&gpt->primary_header);
	init_header(&gpt->backup_header);
	gpt->primary_entries = NULL;
	gpt->backup_entries = NULL;
}

void bootable_gpt_done(struct bootable_gpt *gpt) {
	bootable_free(gpt->primary_entries);
	bootable_free(gpt->backup_entries);
}

int bootable_gpt_find_unused_entry(const struct bootable_gpt *gpt,
                                 bootable_uint32 *entry_index) {

	for (bootable_uint32 i = 0; i < gpt->primary_header.partition_entry_count; i++) {
		struct bootable_gpt_entry *entry = &gpt->primary_entries[i];
		if (!bootable_gpt_entry_is_used(entry)) {
			*entry_index = i;
			return 0;
		}
	}

	return BOOTABLE_ENOSPC;
}

void bootable_gpt_set_disk_uuid(struct bootable_gpt *gpt,
                              const struct bootable_uuid *disk_uuid) {

	bootable_memcpy(gpt->primary_header.disk_uuid.bytes,
	              disk_uuid->bytes,
	              sizeof(disk_uuid->bytes));

	bootable_memcpy(gpt->backup_header.disk_uuid.bytes,
	              disk_uuid->bytes,
	              sizeof(disk_uuid->bytes));
}

int bootable_gpt_format(struct bootable_gpt *gpt,
                      bootable_uint64 disk_size) {

	/* Initialize primary */

	gpt->primary_header.current_lba = 1;
	gpt->primary_header.backup_lba = (disk_size - 512) / 512;
	gpt->primary_header.first_usable_lba = (1 + 1) * 512; /* mbr + gpt header */
	gpt->primary_header.first_usable_lba += GPT_ENTRY_COUNT * GPT_ENTRY_SIZE;
	gpt->primary_header.first_usable_lba /= 512;
	gpt->primary_header.last_usable_lba = (gpt->primary_header.backup_lba * 512);
	gpt->primary_header.last_usable_lba -= (GPT_ENTRY_COUNT * GPT_ENTRY_SIZE);
	gpt->primary_header.last_usable_lba -= 512;
	gpt->primary_header.last_usable_lba /= 512;
	gpt->primary_header.partition_entries_lba = 2;
	gpt->primary_header.partition_entry_count = GPT_ENTRY_COUNT;

	/* Initialize backup header */

	gpt->backup_header.first_usable_lba = gpt->primary_header.first_usable_lba;
	gpt->backup_header.last_usable_lba = gpt->primary_header.last_usable_lba;
	gpt->backup_header.partition_entry_count = GPT_ENTRY_COUNT;
	gpt->backup_header.current_lba = gpt->primary_header.backup_lba;
	gpt->backup_header.backup_lba = gpt->primary_header.current_lba;
	gpt->backup_header.partition_entries_lba = gpt->backup_header.current_lba * 512;
	gpt->backup_header.partition_entries_lba -= GPT_ENTRY_COUNT * GPT_ENTRY_SIZE;
	gpt->backup_header.partition_entries_lba /= 512;

	/* Initialize primary entries */

	struct bootable_gpt_entry *entries;

	entries = bootable_realloc(gpt->primary_entries, BOOTABLE_GPT_ENTRY_COUNT * sizeof(struct bootable_gpt_entry));
	if (entries == NULL) {
		return BOOTABLE_ENOMEM;
	}

	for (bootable_uint32 i = 0; i < BOOTABLE_GPT_ENTRY_COUNT; i++)
		bootable_gpt_entry_init(&entries[i]);

	gpt->primary_entries = entries;

	/* Initialize backup entries */

	entries = bootable_realloc(gpt->backup_entries, BOOTABLE_GPT_ENTRY_COUNT * sizeof(struct bootable_gpt_entry));
	if (entries == NULL) {
		return BOOTABLE_ENOMEM;
	}

	for (bootable_uint32 i = 0; i < BOOTABLE_GPT_ENTRY_COUNT; i++)
		bootable_gpt_entry_init(&entries[i]);

	gpt->backup_entries = entries;

	return 0;
}

int bootable_gpt_import(struct bootable_gpt *gpt,
                      struct bootable_stream *stream) {

	/* TODO : verify checksums */

	/* Read the primary header */

	int err = bootable_stream_set_pos(stream, 512);
	if (err != 0)
		return err;

	err = bootable_gpt_header_import(&gpt->primary_header, stream);
	if (err != 0)
		return err;

	/* Read the primary entries */

	struct bootable_gpt_entry *entries;

	entries = bootable_realloc(gpt->primary_entries, 128 * sizeof(struct bootable_gpt_entry));
	if (entries == NULL)
		return BOOTABLE_ENOMEM;

	gpt->primary_entries = entries;

	err = bootable_stream_set_pos(stream, 1024);
	if (err != 0)
		return err;

	for (bootable_uint32 i = 0; i < BOOTABLE_GPT_ENTRY_COUNT; i++) {

		bootable_gpt_entry_init(&entries[i]);

		err = bootable_gpt_entry_import(&entries[i], stream);
		if (err != 0)
			return err;
	}

	/* Read the backup header */

	err = bootable_stream_set_pos(stream, gpt->primary_header.backup_lba * 512);
	if (err != 0)
		return err;

	err = bootable_gpt_header_import(&gpt->backup_header, stream);
	if (err != 0)
		return err;

	/* Read the backup partition entries. */

	err = bootable_stream_set_pos(stream, gpt->backup_header.partition_entries_lba * 512);
	if (err != 0)
		return err;

	entries = bootable_realloc(gpt->backup_entries, 128 * sizeof(struct bootable_gpt_entry));
	if (entries == NULL)
		return BOOTABLE_ENOMEM;

	gpt->backup_entries = entries;

	for (bootable_uint32 i = 0; i < BOOTABLE_GPT_ENTRY_COUNT; i++) {

		bootable_gpt_entry_init(&entries[i]);

		err = bootable_gpt_entry_import(&entries[i], stream);
		if (err != 0)
			return err;
	}

	return 0;
}

int bootable_gpt_export(const struct bootable_gpt *gpt,
                      struct bootable_stream *stream) {

	int err = bootable_stream_set_pos(stream, 512);
	if (err != 0)
		return err;

	err = bootable_gpt_header_export(&gpt->primary_header, stream);
	if (err != 0)
		return err;

	for (bootable_uint32 i = 0; i < gpt->primary_header.partition_entry_count; i++) {
		err = bootable_gpt_entry_export(&gpt->primary_entries[i], stream);
		if (err != 0)
			return err;
	}

	err = bootable_stream_set_pos(stream, gpt->backup_header.partition_entries_lba * 512);
	if (err != 0)
		return err;

	for (bootable_uint32 i = 0; i < gpt->backup_header.partition_entry_count; i++) {
		err = bootable_gpt_entry_export(&gpt->backup_entries[i], stream);
		if (err != 0)
			return err;
	}

	err = bootable_gpt_header_export(&gpt->backup_header, stream);
	if (err != 0)
		return err;

	err = calculate_checksums(stream);
	if (err != 0)
		return err;

	return 0;
}

const struct bootable_gpt_entry *bootable_gpt_get_entry(const struct bootable_gpt *gpt,
                                                    bootable_uint32 entry_index) {

	if (entry_index >= gpt->primary_header.partition_entry_count)
		return NULL;
	else
		return &gpt->primary_entries[entry_index];
}

int bootable_gpt_get_partition_offset(const struct bootable_gpt *gpt,
                                    bootable_uint32 entry_index,
                                    bootable_uint64 *offset) {

	if ((entry_index > gpt->primary_header.partition_entry_count)
	 || (entry_index > gpt->backup_header.partition_entry_count))
		return BOOTABLE_EINVAL;

	*offset = bootable_gpt_entry_get_offset(&gpt->primary_entries[entry_index]);

	return 0;
}

int bootable_gpt_get_partition_size(const struct bootable_gpt *gpt,
                                  bootable_uint32 entry_index,
                                  bootable_uint64 *size) {

	if ((entry_index > gpt->primary_header.partition_entry_count)
	 || (entry_index > gpt->backup_header.partition_entry_count))
		return BOOTABLE_EINVAL;

	*size = bootable_gpt_entry_get_size(&gpt->primary_entries[entry_index]);

	return 0;
}

int bootable_gpt_set_entry_name(struct bootable_gpt *gpt,
                              bootable_uint32 entry_index,
                              const bootable_uint16 *name) {

	if ((entry_index > gpt->primary_header.partition_entry_count)
	 || (entry_index > gpt->backup_header.partition_entry_count))
		return BOOTABLE_EINVAL;

	bootable_uint64 i = 0;

	while ((name[i] != 0) && (i < 35)) {
		gpt->primary_entries[entry_index].name[i] = name[i];
		gpt->backup_entries[entry_index].name[i] = name[i];
		i++;
	}

	gpt->primary_entries[entry_index].name[i] = 0;
	gpt->backup_entries[entry_index].name[i] = 0;

	return 0;
}

int bootable_gpt_set_entry_name_utf8(struct bootable_gpt *gpt,
                                   bootable_uint32 entry_index,
                                   const char *name) {

	if ((entry_index > gpt->primary_header.partition_entry_count)
	 || (entry_index > gpt->backup_header.partition_entry_count))
		return BOOTABLE_EINVAL;

	bootable_uint64 i = 0;

	while ((name[i] != 0) && (i < 35)) {
		/* TODO : properly convert to UTF-16 */
		gpt->primary_entries[entry_index].name[i] = name[i];
		gpt->backup_entries[entry_index].name[i] = name[i];
		i++;
	}

	gpt->primary_entries[entry_index].name[i] = 0;
	gpt->backup_entries[entry_index].name[i] = 0;

	return 0;
}

int bootable_gpt_set_entry_type(struct bootable_gpt *gpt,
                              bootable_uint32 entry_index,
                              const char *type_uuid_str) {

	if ((entry_index > gpt->primary_header.partition_entry_count)
	 || (entry_index > gpt->backup_header.partition_entry_count))
		return BOOTABLE_EINVAL;

	struct bootable_uuid type_uuid;

	int err = bootable_uuid_parse(&type_uuid, type_uuid_str);
	if (err != 0)
		return err;

	bootable_uuid_copy(&gpt->primary_entries[entry_index].type_uuid, &type_uuid);

	bootable_uuid_copy(&gpt->backup_entries[entry_index].type_uuid, &type_uuid);

	return 0;
}

int bootable_gpt_set_entry_size(struct bootable_gpt *gpt,
                              bootable_uint32 entry_index,
                              bootable_uint64 size) {

	if ((entry_index > gpt->primary_header.partition_entry_count)
	 || (entry_index > gpt->backup_header.partition_entry_count))
		return BOOTABLE_EINVAL;

	/* TODO : sort entries here */

	bootable_uint64 first_lba = gpt->primary_header.first_usable_lba;

	bootable_uint64 lba_count = (size + 511) / 512;
	if (lba_count == 0)
		lba_count = 1;

	bootable_uint64 last_lba = first_lba + lba_count - 1;

	for (bootable_uint64 i = 0; i < gpt->primary_header.partition_entry_count; i++) {

		struct bootable_gpt_entry *entry = &gpt->primary_entries[i];

		if (!bootable_gpt_entry_is_used(entry))
			continue;

		if ((first_lba >= entry->first_lba)
		 || (last_lba >= entry->first_lba)) {
			first_lba = entry->last_lba + 1;
			last_lba = first_lba + lba_count - 1;
		}
	}

	if (last_lba > gpt->primary_header.last_usable_lba)
		return BOOTABLE_ENOSPC;

	gpt->primary_entries[entry_index].first_lba = first_lba;
	gpt->primary_entries[entry_index].last_lba = last_lba;

	gpt->backup_entries[entry_index].first_lba = first_lba;
	gpt->backup_entries[entry_index].last_lba = last_lba;

	return 0;
}
