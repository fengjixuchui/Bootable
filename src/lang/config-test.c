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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void test_parse(void) {

	const char source[] = "arch: x86_64               \n"
	                      "partition_scheme: gpt      \n"
	                      "bootsector: mbr            \n"
	                      "disk_size: 2M              \n"
	                      "fs_loader: true            \n"
	                      "partitions: [              \n"
	                      "  { name: p1, size: 2K },  \n"
	                      "  { name: p2, size: 1M }   \n"
	                      "]                          \n";

	struct bootable_config config;

	struct bootable_syntax_error error;

	bootable_config_init(&config);

	int err = bootable_config_parse(&config, source, &error);

	if (err != 0) {
		fprintf(stderr, "<source>:%lu:%lu: %s\n", error.line, error.column, error.desc);
	}

	assert(err == 0);
	assert(config.arch == BOOTABLE_ARCH_x86_64);
	assert(config.bootsector == BOOTABLE_BOOTSECTOR_MBR);
	assert(config.partition_scheme == BOOTABLE_PARTITION_SCHEME_GPT);
	assert(config.fs_loader);
	assert(config.disk_size == (2 * 1024 * 1024));

	assert(config.partition_count == 2);

	assert(config.partitions[0].name_size == 2);
	assert(memcmp(config.partitions[0].name, "p1", 2) == 0);
	assert(config.partitions[0].size == 2048);
	assert(config.partitions[0].size_specified);
	assert(!config.partitions[0].offset_specified);

	assert(config.partitions[1].name_size == 2);
	assert(memcmp(config.partitions[1].name, "p2", 2) == 0);
	assert(config.partitions[1].size == (1 * 1024 * 1024));
	assert(config.partitions[0].size_specified);
	assert(!config.partitions[0].offset_specified);
}

int main(void) {
	test_parse();
	return EXIT_SUCCESS;
}
