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

#ifndef BOOTABLE_CONFIG_H
#define BOOTABLE_CONFIG_H

#include <bootable/core/arch.h>
#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bootable_syntax_error;

/** Describes which bootsector
 * that is contained by a Pure64 image.
 * @ingroup lang-api
 * */

enum bootable_bootsector {
	/** This means that there is no
	 * bootsector on the disk. */
	BOOTABLE_BOOTSECTOR_NONE,
	/** This means that the bootsector
	 * is MBR formatted. */
	BOOTABLE_BOOTSECTOR_MBR,
	/** This means that the bootsector
	 * is made for PXE booting. */
	BOOTABLE_BOOTSECTOR_PXE,
	/** This means that the bootsector
	 * is made to be loaded as a multiboot
	 * module. */
	BOOTABLE_BOOTSECTOR_MULTIBOOT,
	/** This means that the bootsector
	 * is made to be loaded as a multiboot2
	 * module. */
	BOOTABLE_BOOTSECTOR_MULTIBOOT2
};

/** Gets the size of a specified bootsector.
 * @param bootsector The bootsector to get the size of.
 * @returns The size, in bytes, of the specified bootsector.
 * @ingroup lang-api
 * */

unsigned long int bootable_bootsector_size(enum bootable_bootsector bootsector);

/** Describes a partitioning
 * scheme supported by Pure64.
 * @ingroup lang-api
 * */

enum bootable_partition_scheme {
	/** This means there is not
	 * a partitioning scheme. */
	BOOTABLE_PARTITION_SCHEME_NONE,
	/** This means the partitioning
	 * scheme is GPT. */
	BOOTABLE_PARTITION_SCHEME_GPT
};

/** Contains information about a partition
 * that the user is requesting to create.
 * @ingroup lang-api
 * */

struct bootable_config_partition {
	/** The name of the partition.
	 * This field is optional. */
	const char *name;
	/** The number of characters in the partition name. */
	bootable_size name_size;
	/** The file containing the partition data.
	 * This field is optional. */
	const char *file;
	/** The number of characters in the file path. */
	bootable_size file_size;
	/** The size of the partition on disk. */
	bootable_size size;
	/** Indicates whether or not the size was specified. */
	bootable_bool size_specified;
	/** The offset of the partition on disk.
	 * This field is required if the disk has
	 * not partition format. */
	bootable_size offset;
	/** Indicates whether or not the offset was
	 * specified. */
	bootable_bool offset_specified;
};

/** Represents a disk configuration file.
 * The configuration is used for detecting
 * invalid settings and determining offsets
 * of various code segments and data.
 * @ingroup lang-api
 * */

struct bootable_config {
	/** The architecture of the image. */
	enum bootable_arch arch;
	/** The bootsector used for the disk image. */
	enum bootable_bootsector bootsector;
	/** The partitioning scheme used by the disk image. */
	enum bootable_partition_scheme partition_scheme;
	/** Indicates whether or not the file system loader
	 * is put on the disk. */
	bootable_bool fs_loader;
	/** The size, in bytes, of the disk. */
	bootable_size disk_size;
	/** The size, in bytes, to reserve for the file system. */
	bootable_size fs_size;
	/** The path of the kernel to load. This option
	 * is only valid if the stage three loader is
	 * specified to load a kernel. */
	char *kernel_path;
	/** The root path to Pure64's bootsectors,
	 * initialization code, and more. */
	char *resource_path;
	/** A array of partitions that the user wants to make. */
	struct bootable_config_partition *partitions;
	/** The number of partitions in the partition array. */
	bootable_size partition_count;
};

/** Initializes a configuration file with default values.
 * @param config The configuration structure to initialize.
 * @ingroup lang-api
 * */

void bootable_config_init(struct bootable_config *config);

/** Releases memory allocated by a configuration.
 * @param config An initialized config structure.
 * @ingroup lang-api
 * */

void bootable_config_done(struct bootable_config *config);

/** Parses a configuration file.
 * @param config An initialized configuration structure.
 * @param source A null-terminated configuration file.
 * @param error An error structure that will describe
 * a syntax error if one occurs.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_config_parse(struct bootable_config *config,
                        const char *source,
                        struct bootable_syntax_error *error);

/** Loads a configuration from a file.
 * @param config An initialized config structure.
 * @param filename The path of the configuration file.
 * @param error A pointer to an error structure that
 * will be describe an error if one occurs.
 * @returns Zero on success, an error code on failure.
 * @ingroup lang-api
 * */

int bootable_config_load(struct bootable_config *config,
                       const char *filename,
                       struct bootable_syntax_error *error);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_CONFIG_H */
