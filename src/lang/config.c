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

#include <bootable/lang/config.h>

#include <bootable/lang/syntax-error.h>
#include <bootable/lang/parser.h>
#include <bootable/lang/var.h>
#include <bootable/core/error.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char file_exists(const char *path) {

	FILE *file = fopen(path, "rb");

	if (file == NULL)
		return 0;

	fclose(file);

	return 1;
}

static int parse_size(const struct bootable_value *value,
                      bootable_size *size_ptr) {

	if (value->type != BOOTABLE_VALUE_string) {
		return BOOTABLE_EINVAL;
	}

	const unsigned long int TiB = 1024UL * 1024UL * 1024UL * 1024UL;
	const unsigned long int GiB = 1024UL * 1024UL * 1024UL;
	const unsigned long int MiB = 1024UL * 1024UL;
	const unsigned long int KiB = 1024UL;

	const unsigned long int TB = 1000UL * 1000UL * 1000UL * 1000UL;
	const unsigned long int GB = 1000UL * 1000UL * 1000UL;
	const unsigned long int MB = 1000UL * 1000UL;
	const unsigned long int KB = 1000UL;

	const char *str = value->u.string.data;

	bootable_size str_size = value->u.string.size;

	if (str_size == 0)
		return BOOTABLE_EINVAL;

	char *tmp = malloc(str_size + 1);
	if (tmp == NULL) {
		return BOOTABLE_ENOMEM;
	}

	memcpy(tmp, str, str_size);

	tmp[str_size] = 0;

	bootable_uint64 multiplier = 1;

	const char *suffix = &tmp[str_size - 1];
	if (strcmp(suffix, "T") == 0) {
		multiplier = TiB;
	} else if (strcmp(suffix, "G") == 0) {
		multiplier = GiB;
	} else if (strcmp(suffix, "M") == 0) {
		multiplier = MiB;
	} else if (strcmp(suffix, "K") == 0) {
		multiplier = KiB;
	}

	if (multiplier != 1)
		str_size--;

	if ((multiplier == 1) && (str_size > 2)) {
		suffix = &tmp[str_size - 3];
		if (strcmp(suffix, "TiB") == 0) {
			multiplier = TiB;
		} else if (strcmp(suffix, "GiB") == 0) {
			multiplier = GiB;
		} else if (strcmp(suffix, "MiB") == 0) {
			multiplier = MiB;
		} else if (strcmp(suffix, "KiB") == 0) {
			multiplier = KiB;
		}
		if (multiplier != 1)
			str_size -= 3;
	}

	if ((multiplier == 1) && (str_size > 1)) {
		suffix = &tmp[str_size - 2];
		if (strcmp(suffix, "TB") == 0) {
			multiplier = TB;
		} else if (strcmp(suffix, "GB") == 0) {
			multiplier = GB;
		} else if (strcmp(suffix, "MB") == 0) {
			multiplier = MB;
		} else if (strcmp(suffix, "KB") == 0) {
			multiplier = KB;
		}
		if (multiplier != 1)
			str_size -= 2;
	}

	tmp[str_size] = 0;

	unsigned long int size = 0;

	if (sscanf(tmp, "%lu", &size) != 1) {
		free(tmp);
		return BOOTABLE_EINVAL;
	}

	size *= multiplier;

	if (size_ptr != bootable_null)
		*size_ptr = size;

	free(tmp);

	return 0;
}

static int push_partition(struct bootable_config *config,
                          struct bootable_config_partition *partition) {

	struct bootable_config_partition *partitions = config->partitions;

	bootable_size partition_count = config->partition_count + 1;

	partitions = realloc(partitions, partition_count * sizeof(partitions[0]));
	if (partitions == NULL) {
		return BOOTABLE_ENOMEM;
	}

	partitions[partition_count - 1] = *partition;

	config->partitions = partitions;
	config->partition_count = partition_count;

	return 0;
}

static int handle_bootsector(struct bootable_config *config,
                             const struct bootable_value *value,
                             struct bootable_syntax_error *error) {

	if (value->type != BOOTABLE_VALUE_string) {
		if (error != bootable_null) {
			error->line = value->line;
			error->column = value->column;
			error->desc = "Bootsector value should be a string";
			error->source = bootable_null;
		}
		return BOOTABLE_EINVAL;
	}

	const char *bootsector = value->u.string.data;

	bootable_size bootsector_size = value->u.string.size;

	if ((bootsector_size == 3)
	 && (bootsector[0] == 'm')
	 && (bootsector[1] == 'b')
	 && (bootsector[2] == 'r')) {
		config->bootsector = BOOTABLE_BOOTSECTOR_MBR;
	} else if ((bootsector_size == 3)
	        && (bootsector[0] == 'p')
	        && (bootsector[1] == 'x')
	        && (bootsector[2] == 'e')) {
		config->bootsector = BOOTABLE_BOOTSECTOR_PXE;
	} else if ((bootsector_size == 9)
	        && (memcmp(bootsector, "multiboot", 9) == 0)) {
		config->bootsector = BOOTABLE_BOOTSECTOR_MULTIBOOT;
	} else if ((bootsector_size == 10)
	        && (memcmp(bootsector, "multiboot2", 10) == 0)) {
		config->bootsector = BOOTABLE_BOOTSECTOR_MULTIBOOT2;
	} else {
		if (error != bootable_null) {
			error->line = value->line;
			error->column = value->column;
			error->desc = "Unknown bootsector type";
			error->source = bootable_null;
		}
		return BOOTABLE_EINVAL;
	}

	return 0;
}

static int handle_resource_path(struct bootable_config *config,
                                const struct bootable_value *value,
                                struct bootable_syntax_error *error) {

	if (value->type != BOOTABLE_VALUE_string) {
		if (error != bootable_null) {
			error->line = value->line;
			error->column = value->column;
			error->desc = "Resource path should be a string.";
			error->source = bootable_null;
		}
		return BOOTABLE_EINVAL;
	}

	char *resource_path = malloc(value->u.string.size + 1);
	if (resource_path == NULL) {
		return BOOTABLE_ENOMEM;
	}

	memcpy(resource_path, value->u.string.data, value->u.string.size);
	resource_path[value->u.string.size] = 0;

	free(config->resource_path);
	config->resource_path = resource_path;

	return 0;
}

static int handle_kernel_path(struct bootable_config *config,
                              const struct bootable_value *value,
                              struct bootable_syntax_error *error) {

	if (value->type != BOOTABLE_VALUE_string) {
		if (error != bootable_null) {
			error->line = value->line;
			error->column = value->column;
			error->desc = "Kernel path should be a string.";
			error->source = bootable_null;
		}
		return BOOTABLE_EINVAL;
	}

	char *kernel_path = malloc(value->u.string.size + 1);
	if (kernel_path == NULL) {
		return BOOTABLE_ENOMEM;
	}

	memcpy(kernel_path, value->u.string.data, value->u.string.size);
	kernel_path[value->u.string.size] = 0;

	if (!file_exists(kernel_path)) {
		if (error != bootable_null) {
			error->source = bootable_null;
			error->desc = "Kernel does not exist";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	free(config->kernel_path);
	config->kernel_path = kernel_path;

	return 0;
}

static int handle_fs_loader(struct bootable_config *config,
                            const struct bootable_value *value,
                            struct bootable_syntax_error *error) {

	if (value->type != BOOTABLE_VALUE_boolean) {
		if (error != bootable_null) {
			error->line = value->line;
			error->column = value->column;
			error->desc = "FS Loader switch should be a boolean value.";
			error->source = bootable_null;
		}
		return BOOTABLE_EINVAL;
	}

	config->fs_loader = value->u._bool;

	return 0;
}

static int handle_partition_scheme(struct bootable_config *config,
                                   const struct bootable_value *value,
                                   struct bootable_syntax_error *error) {

	if (value->type != BOOTABLE_VALUE_string) {
		if (error != bootable_null) {
			error->line = value->line;
			error->column = value->column;
			error->desc = "Partition scheme must be a string";
			error->source = bootable_null;
		}
		return BOOTABLE_EINVAL;
	}

	const char *str_data = value->u.string.data;

	bootable_size str_size = value->u.string.size;

	if ((str_size == 4)
	 && (str_data[0] == 'n')
	 && (str_data[1] == 'o')
	 && (str_data[2] == 'n')
	 && (str_data[3] == 'e')) {
		config->partition_scheme = BOOTABLE_PARTITION_SCHEME_NONE;
	} else if ((str_size == 3)
	        && (str_data[0] == 'g')
	        && (str_data[1] == 'p')
	        && (str_data[2] == 't')) {
		config->partition_scheme = BOOTABLE_PARTITION_SCHEME_GPT;
	} else {
		if (error != bootable_null) {
			error->line = value->line;
			error->column = value->column;
			error->desc = "Unknown partition type.";
			error->source = bootable_null;
		}
		return BOOTABLE_EINVAL;
	}

	return 0;
}

static int handle_disk_size(struct bootable_config *config,
                            const struct bootable_value *value,
                            struct bootable_syntax_error *error) {

	if (value->type == BOOTABLE_VALUE_number) {
		config->disk_size = value->u.number;
		return 0;
	} else if (value->type != BOOTABLE_VALUE_string) {
		if (error != bootable_null) {
			error->desc = "Disk size must be a number or a string.";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	if (value->u.string.size == 0) {
		if (error != bootable_null) {
			error->desc = "Disk size not specified";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	if (parse_size(value, &config->disk_size) != 0) {
		if (error != bootable_null) {
			error->desc = "Invalid disk size";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	return 0;
}

static int handle_arch(struct bootable_config *config,
                       const struct bootable_value *value,
                       struct bootable_syntax_error *error) {

	if (value->type != BOOTABLE_VALUE_string) {
		if (error != bootable_null) {
			error->desc = "Architecture value must be a string";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	if ((value->u.string.size == 6)
	 && (value->u.string.data[0] == 'x')
	 && (value->u.string.data[1] == '8')
	 && (value->u.string.data[2] == '6')
	 && (value->u.string.data[3] == '_')
	 && (value->u.string.data[4] == '6')
	 && (value->u.string.data[5] == '4')) {
		config->arch = BOOTABLE_ARCH_x86_64;
		return 0;
	} else {
		if (error != bootable_null) {
			error->desc = "Unsupported architecture";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	return 0;
}

static int handle_fs_size(struct bootable_config *config,
                          const struct bootable_value *value,
                          struct bootable_syntax_error *error) {

	if (value->type == BOOTABLE_VALUE_number) {
		config->fs_size = value->u.number;
		return 0;
	} else if (value->type != BOOTABLE_VALUE_string) {
		if (error != bootable_null) {
			error->desc = "File system size must be a string or number";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	if (value->u.string.size == 0) {
		if (error != bootable_null) {
			error->desc = "File system size not specified";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	if (parse_size(value, &config->fs_size) != 0) {
		if (error != bootable_null) {
			error->desc = "Invalid file system size";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	return 0;
}

static int handle_partition_name(struct bootable_config_partition *partition,
                                 const struct bootable_value *value,
                                 struct bootable_syntax_error *error) {

	if (value->type != BOOTABLE_VALUE_string) {
		if (error != bootable_null) {
			error->desc = "Partition name should be a string";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	partition->name = value->u.string.data;
	partition->name_size = value->u.string.size;

	return 0;
}

static int handle_partition_file(struct bootable_config_partition *partition,
                                 const struct bootable_value *value,
                                 struct bootable_syntax_error *error) {

	if (value->type != BOOTABLE_VALUE_string) {
		if (error != bootable_null) {
			error->desc = "Partition file path should be a string";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	partition->file = value->u.string.data;
	partition->file_size = value->u.string.size;

	return 0;
}

static int handle_partition_size(struct bootable_config_partition *partition,
                                 const struct bootable_value *value,
                                 struct bootable_syntax_error *error) {

	if (value->type == BOOTABLE_VALUE_number) {
		partition->size_specified = bootable_true;
		partition->size = value->u.number;
		return 0;
	} else if (value->type != BOOTABLE_VALUE_string) {
		if (error != bootable_null) {
			error->desc = "Partition name should be a string";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	if (parse_size(value, &partition->size) != 0) {
		if (error != bootable_null) {
			error->desc = "Invalid partition size";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	partition->size_specified = bootable_true;

	return 0;
}

static int handle_partition_offset(struct bootable_config_partition *partition,
                                   const struct bootable_value *value,
                                   struct bootable_syntax_error *error) {

	if (value->type == BOOTABLE_VALUE_number) {
		partition->offset_specified = bootable_true;
		partition->offset = value->u.number;
		return 0;
	} else if (value->type != BOOTABLE_VALUE_string) {
		if (error != bootable_null) {
			error->desc = "Partition offset should be a string or number.";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	if (parse_size(value, &partition->offset) != 0) {
		if (error != bootable_null) {
			error->desc = "Invalid partition size";
			error->line = value->line;
			error->column = value->column;
		}
		return BOOTABLE_EINVAL;
	}

	partition->offset_specified = bootable_true;

	return 0;
}

static int handle_partition(struct bootable_config *config,
                            const struct bootable_object *object,
                            struct bootable_syntax_error *error) {

	struct bootable_config_partition partition;

	partition.name = bootable_null;
	partition.name_size = 0;
	partition.file = bootable_null;
	partition.file_size = 0;
	partition.size = 0;
	partition.size_specified = bootable_false;
	partition.offset = 0;
	partition.offset_specified = bootable_false;

	for (bootable_size i = 0; i < object->var_count; i++) {
		const struct bootable_var *var = &object->var_array[i];
		if (bootable_key_cmp_id(&var->key, "name") == 0) {
			int err = handle_partition_name(&partition, &var->value, error);
			if (err != 0)
				return err;
		} else if (bootable_key_cmp_id(&var->key, "file") == 0) {
			int err = handle_partition_file(&partition, &var->value, error);
			if (err != 0)
				return err;
		} else if (bootable_key_cmp_id(&var->key, "size") == 0) {
			int err = handle_partition_size(&partition, &var->value, error);
			if (err != 0)
				return err;
		} else if (bootable_key_cmp_id(&var->key, "offset") == 0) {
			int err = handle_partition_offset(&partition, &var->value, error);
			if (err != 0)
				return err;
		} else {
			if (error != bootable_null) {
				error->desc = "Invalid partition field";
				error->line = var->key.line;
				error->column = var->key.column;
			}
			return BOOTABLE_EINVAL;
		}
	}

	int err = push_partition(config, &partition);
	if (err != 0)
		return err;

	return 0;
}

static int handle_partitions(struct bootable_config *config,
                             const struct bootable_value *value,
                             struct bootable_syntax_error *error) {

	if (value->type != BOOTABLE_VALUE_list) {
		if (error != bootable_null) {
			error->line = value->line;
			error->column = value->column;
			error->desc = "Partitions variable should be a list";
		}
		return BOOTABLE_EINVAL;
	}

	const struct bootable_list *list = &value->u.list;

	for (bootable_size i = 0; i < list->value_count; i++) {

		const struct bootable_value *value = &list->value_array[i];
		if (value->type != BOOTABLE_VALUE_object) {
			if (error != bootable_null) {
				error->desc = "Partition field should be an object";
				error->line = value->line;
				error->column = value->column;
			}
			return BOOTABLE_EINVAL;
		}

		int err = handle_partition(config, &value->u.object, error);
		if (err != 0)
			return err;
	}

	return 0;
}

static int handle_var(struct bootable_config *config,
                      const struct bootable_var *var,
                      struct bootable_syntax_error *error) {

	if (bootable_var_cmp_id(var, "bootsector") == 0) {
		return handle_bootsector(config, &var->value, error);
	} else if (bootable_var_cmp_id(var, "resource_path") == 0) {
		return handle_resource_path(config, &var->value, error);
	} else if (bootable_var_cmp_id(var, "kernel_path") == 0) {
		return handle_kernel_path(config, &var->value, error);
	} else if (bootable_var_cmp_id(var, "fs_loader") == 0) {
		return handle_fs_loader(config, &var->value, error);
	} else if (bootable_var_cmp_id(var, "partition_scheme") == 0) {
		return handle_partition_scheme(config, &var->value, error);
	} else if (bootable_var_cmp_id(var, "disk_size") == 0) {
		return handle_disk_size(config, &var->value, error);
	} else if (bootable_var_cmp_id(var, "arch") == 0) {
		return handle_arch(config, &var->value, error);
	} else if (bootable_var_cmp_id(var, "fs_size") == 0) {
		return handle_fs_size(config, &var->value, error);
	} else if (bootable_var_cmp_id(var, "partitions") == 0) {
		return handle_partitions(config, &var->value, error);
	} else {
		if (error != bootable_null) {
			error->desc = "Unknown variable";
			error->line = var->key.line;
			error->column = var->key.column;
		}
		return BOOTABLE_EINVAL;
	}

	return 0;
}

static int validate_vars(struct bootable_config *config,
                         struct bootable_syntax_error *error) {

	if (config->arch == BOOTABLE_ARCH_none) {
		error->desc = "Architecture not specified";
		return BOOTABLE_EINVAL;
	}

	if (config->bootsector == BOOTABLE_BOOTSECTOR_NONE) {
		/* Allow the 'mbr' bootsector to be the default one. */
		config->bootsector = BOOTABLE_BOOTSECTOR_MBR;
	}

	if (config->partition_scheme == BOOTABLE_PARTITION_SCHEME_NONE) {
		/* Allow GPT to be the default partition scheme */
		config->partition_scheme = BOOTABLE_PARTITION_SCHEME_GPT;
	}

	if ((config->fs_loader) && (config->bootsector != BOOTABLE_BOOTSECTOR_MBR)) {
		error->desc = "File system loader only valid with MBR bootsector";
		return BOOTABLE_EINVAL;
	}

	if ((!config->fs_loader) && (config->kernel_path == NULL)) {
		error->desc = "Kernel not specifed";
		return BOOTABLE_EINVAL;
	}

	return 0;
}

unsigned long int bootable_bootsector_size(enum bootable_bootsector bootsector) {
	switch (bootsector) {
	case BOOTABLE_BOOTSECTOR_PXE:
		return 1024;
	default:
		return 512;
	}
}

void bootable_config_init(struct bootable_config *config) {
	config->arch = BOOTABLE_ARCH_none;
	config->bootsector = BOOTABLE_BOOTSECTOR_NONE;
	config->partition_scheme = BOOTABLE_PARTITION_SCHEME_NONE;
	config->fs_loader = bootable_false;
	config->disk_size = 1 * 1024 * 1024;
	config->fs_size = 512 * 1024;
	config->kernel_path = bootable_null;
	config->resource_path = bootable_null;
	config->partitions = bootable_null;
	config->partition_count = 0;
}

void bootable_config_done(struct bootable_config *config) {
	free(config->kernel_path);
	free(config->resource_path);
	config->kernel_path = bootable_null;
	config->resource_path = bootable_null;
}

int bootable_config_parse(struct bootable_config *config,
                        const char *source,
                        struct bootable_syntax_error *error) {

	if (error != bootable_null) {
		error->source = bootable_null;
		error->desc = bootable_null;
		error->line = 0;
		error->column = 0;
	}

	struct bootable_parser parser;

	bootable_parser_init(&parser);

	int err = bootable_parser_parse(&parser, source, error);
	if (err != 0) {
		if ((err != BOOTABLE_EINVAL) && (error != bootable_null)) {
			error->desc = "Failed to scan source";
		}
		bootable_parser_done(&parser);
		return err;
	}

	while (!bootable_parser_eof(&parser)) {

		const struct bootable_var *var = bootable_parser_next(&parser);
		if (var == bootable_null)
			break;

		err = handle_var(config, var, error);
		if (err != 0) {
			bootable_parser_done(&parser);
			return err;
		}
	}

	bootable_parser_done(&parser);

	err = validate_vars(config, error);
	if (err != 0) {
		return err;
	}

	return 0;
}

int bootable_config_load(struct bootable_config *config,
                       const char *filename,
                       struct bootable_syntax_error *error) {

	if (error != bootable_null) {
		error->source = bootable_null;
		error->desc = bootable_null;
		error->line = 0;
		error->column = 0;
	}

	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		if (error != bootable_null) {
			error->desc = "Failed to open file";
		}
		return BOOTABLE_EINVAL;
	}

	if (fseek(file, 0UL, SEEK_END) != 0) {
		fclose(file);
		if (error != bootable_null) {
			error->desc = "Failed to seek to end of file";
		}
		return BOOTABLE_EINVAL;
	}

	long int file_size = ftell(file);
	if (file_size == -1L) {
		fclose(file);
		if (error != bootable_null) {
			error->desc = "Failed to tell file position";
		}
		return BOOTABLE_EINVAL;
	}

	if (fseek(file, 0UL, SEEK_SET) != 0) {
		fclose(file);
		if (error != bootable_null) {
			error->desc = "Failed to seek to start of file";
		}
		return BOOTABLE_EINVAL;
	}

	char *source = malloc(file_size + 1);
	if (source == NULL) {
		fclose(file);
		if (error != bootable_null) {
			error->desc = "Failed to allocate file memory";
		}
		return BOOTABLE_ENOMEM;
	}

	source[file_size] = 0;

	if (fread(source, 1, file_size, file) != ((size_t) file_size)) {
		free(source);
		fclose(file);
		if (error != bootable_null) {
			error->desc = "Failed to read file";
		}
		return BOOTABLE_EIO;
	}

	fclose(file);

	int err = bootable_config_parse(config, source, error);

	free(source);

	return err;
}
