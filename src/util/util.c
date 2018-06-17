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

#include "util.h"

#include <bootable/lang/config.h>

#include <bootable/core/error.h>
#include <bootable/core/gpt.h>
#include <bootable/core/mbr.h>
#include <bootable/core/partition.h>
#include <bootable/core/types.h>
#include <bootable/lang/syntax-error.h>

#include <stdlib.h>
#include <string.h>

#ifndef BOOTABLE_INSTALL_PATH
#define BOOTABLE_INSTALL_PATH "/opt/return-infinity"
#endif

#ifndef BOOTABLE_RESOURCE_PATH
#define BOOTABLE_RESOURCE_PATH BOOTABLE_INSTALL_PATH "/share/bootable/resources"
#endif

#ifndef BOOTABLE_SIZE
#define BOOTABLE_SIZE 4096
#endif

const unsigned long int bootable_data_size = BOOTABLE_SIZE;

struct file_buf {
	void *data;
	unsigned long int size;
};

static int file_open(struct file_buf *file_buf,
                       const char *path) {

	file_buf->data = NULL;
	file_buf->size = 0;

	FILE *kernel = fopen(path, "rb");
	if (kernel == NULL)
		return BOOTABLE_ENOENT;

	if (fseek(kernel, 0UL, SEEK_END) != 0) {
		fclose(kernel);
		return BOOTABLE_EINVAL;
	}

	long int kernel_size = ftell(kernel);
	if (kernel_size == -1L) {
		fclose(kernel);
		return BOOTABLE_EINVAL;
	}

	if (fseek(kernel, 0UL, SEEK_SET) != 0) {
		fclose(kernel);
		return BOOTABLE_EINVAL;
	}

	void *buf = malloc(kernel_size);
	if (buf == NULL) {
		fclose(kernel);
		return BOOTABLE_ENOMEM;
	}

	if (fread(buf, 1, kernel_size, kernel) != ((size_t) kernel_size)) {
		fclose(kernel);
		free(buf);
		return BOOTABLE_EIO;
	}

	fclose(kernel);

	file_buf->data = buf;
	file_buf->size = (unsigned long int) kernel_size;

	return 0;
}

static void file_done(struct file_buf *file_buf) {
	free(file_buf->data);
	file_buf->data = NULL;
	file_buf->size = 0;
}

static const char *get_root_resource_path(const struct bootable_config *config) {

	if (config->resource_path == NULL) {

		const char *respath = getenv("BOOTABLE_RESOURCE_PATH");
		if (respath != NULL)
			return respath;

		return BOOTABLE_RESOURCE_PATH;

	} else {
		return config->resource_path;
	}
}

char *get_full_resource_path(const struct bootable_config *config,
                             const char *suffix_path) {

	const char *resource_prefix = get_root_resource_path(config);
	if (resource_prefix == NULL)
		return NULL;

	unsigned long int path_size = 0;
	path_size += strlen(resource_prefix);
	path_size += 1; /* for '/' */
	path_size += strlen(suffix_path);

	char *full_resource_path = malloc(path_size + 1);
	if (full_resource_path == NULL)
		return NULL;

	strcpy(full_resource_path, resource_prefix);
	strcat(full_resource_path, "/");
	strcat(full_resource_path, suffix_path);

	return full_resource_path;
}

static int resource_get_size(const struct bootable_config *config,
                             bootable_uint64 *size,
                             const char *suffix_path) {

	char *full_path = get_full_resource_path(config, suffix_path);
	if (full_path == NULL)
		return BOOTABLE_ENOMEM;

	FILE *file = fopen(full_path, "rb");
	if (file == NULL) {
		free(full_path);
		return BOOTABLE_ENOENT;
	}

	free(full_path);

	int err = fseek(file, 0, SEEK_END);
	if (err != 0) {
		fclose(file);
		return BOOTABLE_EINVAL;
	}

	long int file_size = ftell(file);
	if (file_size == -1L) {
		fclose(file);
		return BOOTABLE_EINVAL;
	}

	fclose(file);

	if (size != NULL)
		*size = (bootable_uint64) file_size;

	return 0;
}

static int resource_open(const struct bootable_config *config,
                         struct file_buf *file_buf,
                         const char *suffix_path) {

	char *full_path = get_full_resource_path(config, suffix_path);
	if (full_path == NULL)
		return BOOTABLE_ENOMEM;

	int err = file_open(file_buf, full_path);
	if (err != 0) {
		free(full_path);
		return err;
	}

	free(full_path);

	return 0;
}

static int bootsector_open(const struct bootable_config *config,
                           struct file_buf *file_buf,
                           enum bootable_bootsector bootsector) {

	switch (bootsector) {
	case BOOTABLE_BOOTSECTOR_MBR:
		return resource_open(config, file_buf, "x86_64/bootsectors/mbr.sys");
	case BOOTABLE_BOOTSECTOR_PXE:
		return resource_open(config, file_buf, "x86_64/bootsectors/pxestart.sys");
	case BOOTABLE_BOOTSECTOR_MULTIBOOT:
		return resource_open(config, file_buf, "x86_64/bootsectors/mulitboot.sys");
	case BOOTABLE_BOOTSECTOR_MULTIBOOT2:
		return resource_open(config, file_buf, "x86_64/bootsectors/mulitboot2.sys");
	default:
		break;
	}

	return BOOTABLE_EINVAL;
}

static int write_bootsector(struct bootable_util *util) {

	int err = bootable_stream_set_pos(&util->disk_file.base, 0);
	if (err != 0)
		return err;

	struct file_buf file_buf;

	err = bootsector_open(&util->config, &file_buf, util->config.bootsector);
	if (err != 0) {
		fprintf(util->errlog, "Failed to open bootsector: %s\n", bootable_strerror(err));
		return err;
	}

	err = bootable_stream_write(&util->disk_file.base, file_buf.data, file_buf.size);
	if (err != 0) {
		fprintf(util->errlog, "Failed to write bootsector: %s\n", bootable_strerror(err));
		file_done(&file_buf);
		return err;
	}

	file_done(&file_buf);

	return 0;
}

static int write_kernel_bin(struct bootable_util *util) {

	const char *path = util->config.kernel_path;
	if (path == NULL)
		path = "kernel";

	struct file_buf file_buf;

	int err = file_open(&file_buf, path);
	if (err != 0) {
		fprintf(util->errlog, "Failed to open '%s'.\n", path);
		file_done(&file_buf);
		return err;
	}

	unsigned long int kernel_offset = 0;
	kernel_offset += bootable_bootsector_size(util->config.bootsector);
	kernel_offset += bootable_data_size;

	err = bootable_stream_set_pos(&util->disk_file.base, kernel_offset);
	if (err != 0) {
		file_done(&file_buf);
		return err;
	}

	err = bootable_stream_write(&util->disk_file.base, file_buf.data, file_buf.size);
	if (err != 0) {
		file_done(&file_buf);
		return err;
	}

	file_done(&file_buf);

	return 0;
}

static int write_stage_two_bin(struct bootable_util *util) {

	unsigned long int stage_two_offset = 0;

	stage_two_offset += bootable_bootsector_size(util->config.bootsector);

	int err = bootable_stream_set_pos(&util->disk_file.base, stage_two_offset);
	if (err != 0)
		return err;

	struct file_buf file_buf;

	err = resource_open(&util->config, &file_buf, "x86_64/bootable.sys");
	if (err != 0) {
		fprintf(util->errlog, "Failed to open 2nd stage bootloader file.");
		return err;
	}

	err = bootable_stream_write(&util->disk_file.base, file_buf.data, file_buf.size);
	if (err != 0) {
		file_done(&file_buf);
		return err;
	}

	file_done(&file_buf);

	return 0;
}

static int write_flat_partition(struct bootable_util *util) {

	int err = write_stage_two_bin(util);
	if (err != 0)
		return err;

	if (!util->config.fs_loader) {
		err = write_kernel_bin(util);
		if (err != 0)
			return err;
	} else {
		/* TODO : file system loader with flat partition */
		return BOOTABLE_ENOSYS;
	}

	return 0;
}

static int write_fs_gpt(struct bootable_util *util,
                        struct bootable_gpt *gpt) {

	if (!util->config.fs_loader)
		return 0;

	int err = bootable_gpt_set_entry_type(gpt, 2, BOOTABLE_UUID_FILE_SYSTEM);
	if (err != 0)
		return err;

	err = bootable_gpt_set_entry_name(gpt, 2, u"Pure64 File System");
	if (err != 0)
		return err;

	err = bootable_gpt_set_entry_size(gpt, 2, util->config.fs_size);
	if (err != 0)
		return err;

	struct bootable_partition partition;

	bootable_partition_init(&partition);

	bootable_partition_set_offset(&partition, gpt->primary_entries[2].first_lba * 512);

	bootable_partition_set_size(&partition, util->config.fs_size);

	bootable_partition_set_disk(&partition, &util->disk_file.base);

	err = bootable_fs_make_dir(&util->fs, "/boot");
	if (err != 0)
		return err;

	err = bootable_fs_export(&util->fs, &partition.stream);
	if (err != 0)
		return err;

	return 0;
}

static int write_kernel_gpt(struct bootable_util *util,
                            struct bootable_gpt *gpt) {

	int err = bootable_gpt_set_entry_type(gpt, 1, BOOTABLE_UUID_STAGE_THREE);
	if (err != 0)
		return err;

	err = bootable_gpt_set_entry_name(gpt, 1, u"Pure64 Kernel");
	if (err != 0)
		return err;

	const char *kernel_path = util->config.kernel_path;
	if (kernel_path == bootable_null)
		kernel_path = "kernel";

	struct file_buf kernel;

	err = file_open(&kernel, kernel_path);
	if (err != 0) {
		fprintf(util->errlog, "Failed to open '%s'.\n", kernel_path);
		return err;
	}

	err = bootable_gpt_set_entry_size(gpt, 1, kernel.size);
	if (err != 0) {
		file_done(&kernel);
		return err;
	}

	err = bootable_stream_set_pos(&util->disk_file.base, gpt->primary_entries[1].first_lba * 512);
	if (err != 0) {
		file_done(&kernel);
		return err;
	}

	err = bootable_stream_write(&util->disk_file.base, kernel.data, kernel.size);
	if (err != 0) {
		file_done(&kernel);
		return err;
	}

	file_done(&kernel);

	return 0;
}

static int write_loader_gpt(struct bootable_util *util,
                            struct bootable_gpt *gpt) {

	int err = bootable_gpt_set_entry_type(gpt, 1, BOOTABLE_UUID_STAGE_THREE);
	if (err != 0)
		return err;

	err = bootable_gpt_set_entry_name(gpt, 1, u"Pure64 FS Loader");
	if (err != 0)
		return err;

	struct file_buf file_buf;

	err = resource_open(&util->config, &file_buf, "x86_64/fs-loader.sys");
	if (err != 0) {
		fprintf(util->errlog, "Failed to open file system loader.\n");
		return err;
	}

	err = bootable_gpt_set_entry_size(gpt, 1, file_buf.size);
	if (err != 0) {
		file_done(&file_buf);
		return err;
	}

	err = bootable_stream_set_pos(&util->disk_file.base, gpt->primary_entries[1].first_lba * 512);
	if (err != 0) {
		file_done(&file_buf);
		return err;
	}

	err = bootable_stream_write(&util->disk_file.base, file_buf.data, file_buf.size);
	if (err != 0) {
		file_done(&file_buf);
		return err;
	}

	file_done(&file_buf);

	return 0;
}

static int write_stage_three_gpt(struct bootable_util *util,
                                 struct bootable_gpt *gpt) {

	if (util->config.fs_loader)
		return write_loader_gpt(util, gpt);
	else
		return write_kernel_gpt(util, gpt);
}

static int write_stage_two_gpt(struct bootable_util *util,
                               struct bootable_gpt *gpt) {

	int err = bootable_gpt_set_entry_type(gpt, 0, BOOTABLE_UUID_STAGE_TWO);
	if (err != 0)
		return err;

	err = bootable_gpt_set_entry_name(gpt, 0, u"Pure64 Stage Two");
	if (err != 0)
		return err;

	struct file_buf file_buf;

	err = resource_open(&util->config, &file_buf, "x86_64/bootable.sys");
	if (err != 0) {
		fprintf(util->errlog, "Failed to open 2nd stage bootloader file.");
		return err;
	}

	err = bootable_gpt_set_entry_size(gpt, 0, bootable_data_size);
	if (err != 0) {
		file_done(&file_buf);
		return err;
	}

	err = bootable_stream_set_pos(&util->disk_file.base, gpt->primary_entries[0].first_lba * 512);
	if (err != 0) {
		file_done(&file_buf);
		return err;
	}

	err = bootable_stream_write(&util->disk_file.base, file_buf.data, file_buf.size);
	if (err != 0) {
		file_done(&file_buf);
		return err;
	}

	file_done(&file_buf);

	return 0;
}

static int update_mbr_gpt(struct bootable_util *util,
                          struct bootable_gpt *gpt) {

	bootable_uint64 bootable_data_size = 0;

	bootable_uint64 stage_three_data_size = 0;

	int err = 0;

	err = resource_get_size(&util->config, &bootable_data_size, "x86_64/bootable.sys");
	if (err != 0) {
		fprintf(util->errlog, "Failed to get stage two bootloader size.\n");
		return err;
	}

	err = resource_get_size(&util->config, &stage_three_data_size, "x86_64/fs-loader.sys");
	if (err != 0) {
		fprintf(util->errlog, "Failed to get file system loader size.\n");
		return err;
	}

	struct bootable_mbr mbr;

	bootable_mbr_zero(&mbr);

	err = bootable_mbr_read(&mbr, &util->disk_file.base);
	if (err != 0)
		return err;

	mbr.st2dap.sector = gpt->primary_entries[0].first_lba;
	mbr.st2dap.sector_count = (bootable_data_size + 511) / 512;

	mbr.st3dap.sector = gpt->primary_entries[1].first_lba;
	mbr.st3dap.sector_count = (stage_three_data_size + 511) / 512;

	err = bootable_mbr_write(&mbr, &util->disk_file.base);
	if (err != 0)
		return err;

	return 0;
}

static int write_gpt_config_partition(struct bootable_util *util,
                                      struct bootable_gpt *gpt,
                                      const struct bootable_config_partition *partition) {

	bootable_uint32 entry_index = 0;

	int err = bootable_gpt_find_unused_entry(gpt, &entry_index);
	if (err != 0)
		return err;

	const char *dummy_uuid = "6e65efa4-cfde-44cb-82a3-13d4c396e04c";

	err = bootable_gpt_set_entry_type(gpt, entry_index, dummy_uuid);
	if (err != 0)
		return err;

	err = bootable_gpt_set_entry_name_utf8(gpt, entry_index, partition->name);
	if (err != 0)
		return err;

	(void) util;

	/* TODO : give new partition a size. */
	/* TODO : allocate partition size. */
	/* TODO : write/update partition data. */

	return 0;
}

static int write_gpt_partitions(struct bootable_util *util) {

	struct bootable_gpt gpt;

	bootable_gpt_init(&gpt);

	int err = bootable_gpt_format(&gpt, util->config.disk_size);
	if (err != 0) {
		bootable_gpt_done(&gpt);
		return err;
	}

	err = write_stage_two_gpt(util, &gpt);
	if (err != 0) {
		bootable_gpt_done(&gpt);
		return err;
	}

	err = write_stage_three_gpt(util, &gpt);
	if (err != 0) {
		bootable_gpt_done(&gpt);
		return err;
	}

	err = write_fs_gpt(util, &gpt);
	if (err != 0) {
		bootable_gpt_done(&gpt);
		return err;
	}

	for (bootable_size i = 0; i < util->config.partition_count; i++) {
		err = write_gpt_config_partition(util, &gpt, &util->config.partitions[i]);
		if (err != 0)
			return err;
	}

	err = update_mbr_gpt(util, &gpt);
	if (err != 0) {
		bootable_gpt_done(&gpt);
		return err;
	}

	err = bootable_gpt_export(&gpt, &util->disk_file.base);
	if (err != 0) {
		bootable_gpt_done(&gpt);
		return err;
	}

	bootable_gpt_done(&gpt);

	return 0;
}

static int write_partitions(struct bootable_util *util) {

	enum bootable_partition_scheme partition_scheme = util->config.partition_scheme;

	int err = 0;

	switch (partition_scheme) {
	case BOOTABLE_PARTITION_SCHEME_NONE:
		err = write_flat_partition(util);
		if (err != 0)
			return err;
		break;
	case BOOTABLE_PARTITION_SCHEME_GPT:
		err = write_gpt_partitions(util);
		if (err != 0)
			return err;
		break;
	}

	return 0;
}

static int import_fs_gpt(struct bootable_util *util,
                         struct bootable_gpt *gpt) {

	if (!util->config.fs_loader)
		return 0;

	struct bootable_partition partition;

	bootable_partition_init(&partition);

	bootable_partition_set_offset(&partition, gpt->primary_entries[2].first_lba * 512);

	bootable_partition_set_size(&partition, util->config.fs_size);

	bootable_partition_set_disk(&partition, &util->disk_file.base);

	int err = bootable_fs_import(&util->fs, &partition.stream);
	if (err != 0)
		return err;

	return 0;
}

static int import_gpt(struct bootable_util *util) {

	struct bootable_gpt gpt;

	bootable_gpt_init(&gpt);

	int err = bootable_gpt_import(&gpt, &util->disk_file.base);
	if (err != 0)
		return err;

	err = import_fs_gpt(util, &gpt);
	if (err != 0) {
		bootable_gpt_done(&gpt);
		return err;
	}

	bootable_gpt_done(&gpt);

	return 0;
}

static int save_fs_gpt(struct bootable_util *util,
                       struct bootable_gpt *gpt) {

	struct bootable_partition partition;

	bootable_partition_init(&partition);

	bootable_partition_set_offset(&partition, gpt->primary_entries[2].first_lba * 512);

	bootable_partition_set_size(&partition, util->config.fs_size);

	bootable_partition_set_disk(&partition, &util->disk_file.base);

	int err = bootable_fs_export(&util->fs, &partition.stream);
	if (err != 0)
		return err;

	return 0;
}

static int save_gpt(struct bootable_util *util) {

	struct bootable_gpt gpt;

	bootable_gpt_init(&gpt);

	int err = bootable_gpt_import(&gpt, &util->disk_file.base);
	if (err != 0) {
		bootable_gpt_done(&gpt);
		return err;
	}

	err = save_fs_gpt(util, &gpt);
	if (err != 0) {
		bootable_gpt_done(&gpt);
		return err;
	}

	err = bootable_gpt_export(&gpt, &util->disk_file.base);
	if (err != 0) {
		bootable_gpt_done(&gpt);
		return err;
	}

	bootable_gpt_done(&gpt);

	return 0;
}

void bootable_util_init(struct bootable_util *util) {
	bootable_config_init(&util->config);
	bootable_fstream_init(&util->disk_file);
	bootable_fs_init(&util->fs);
	util->errlog = stderr;
}

void bootable_util_done(struct bootable_util *util) {
	bootable_config_done(&util->config);
	bootable_fstream_done(&util->disk_file);
	bootable_fs_free(&util->fs);
}

int bootable_util_create_disk(struct bootable_util *util,
                            const char *path) {

	int err = bootable_fstream_open(&util->disk_file, path, "wb+");
	if (err != 0)
		return err;

	err = bootable_fstream_zero(&util->disk_file, util->config.disk_size);
	if (err != 0)
		return err;

	err = write_bootsector(util);
	if (err != 0)
		return err;

	err = write_partitions(util);
	if (err != 0)
		return err;

	return 0;
}

int bootable_util_open_config(struct bootable_util *util,
                            const char *path) {

	struct bootable_syntax_error error;

	int err = bootable_config_load(&util->config, path, &error);
	if (err != 0) {
		if ((error.line > 0) && (error.column > 0))
			fprintf(util->errlog, "%s:%lu:%lu: %s\n", path, error.line, error.column, error.desc);
		else if (error.line > 0)
			fprintf(util->errlog, "%s:%lu: %s\n", path, error.line, error.desc);
		else
			fprintf(util->errlog, "%s: %s\n", path, error.desc);
		return err;
	}

	return 0;
}

int bootable_util_open_disk(struct bootable_util *util,
                            const char *path) {

	int err = bootable_fstream_open(&util->disk_file, path, "rb+");
	if (err != 0)
		return err;

	if (util->config.partition_scheme == BOOTABLE_PARTITION_SCHEME_GPT) {
		err = import_gpt(util);
		if (err != 0)
			return err;
	}

	return 0;
}

int bootable_util_save_disk(struct bootable_util *util) {

	if (!util->config.fs_loader)
		return 0;

	if (util->config.partition_scheme == BOOTABLE_PARTITION_SCHEME_GPT)
		return save_gpt(util);

	return 0;
}
