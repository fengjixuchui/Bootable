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

#include <bootable/core/error.h>
#include <bootable/core/file.h>
#include <bootable/core/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* * * * * * * * * * * * * *
 * Command Line Declarations
 * * * * * * * * * * * * * */

/** Compares an option with a command
 * line argument. This function checks
 * both short and long options.
 * @param arg The argument from the command line.
 * @param opt The long option (the two '--' are added
 * automatically).
 * @param s_opt The letter representing the short option.
 * @returns True if the argument is a match, false
 * if the argument is not a match.
 * */

static bootable_bool check_opt(const char *arg,
                             const char *opt,
                             char s_opt) {

	/* create a string version
	 * of the short option */
	char s_opt3[3];
	s_opt3[0] = '-';
	s_opt3[1] = s_opt;
	s_opt3[2] = 0;
	if (strcmp(s_opt3, arg) == 0)
		return bootable_true;

	if ((arg[0] == '-')
	 && (arg[1] == '-')
	 && (strcmp(&arg[2], opt) == 0))
		return bootable_true;

	return bootable_false;
}

/** Prints the help message.
 * @param argv0 The name of the program.
 * */

static void print_help(const char *argv0) {
	printf("Usage: %s [options] <command>\n", argv0);
	printf("\n");
	printf("Options:\n");
	printf("\t--disk,   -d : Specify the path to the disk file.\n");
	printf("\t--config, -c : Specify the path to the config file.\n");
	printf("\t--help,   -h : Print this help message.\n");
	printf("\n");
	printf("Commands:\n");
	printf("\tinit  : Initialize the disk image.\n");
	printf("\tcat   : Print the contents of a file.\n");
	printf("\tcp    : Copy file from host file system to Pure64 image.\n");
	printf("\tls    : List directory contents.\n");
	printf("\tmkdir : Create a directory.\n");
}

static bootable_bool is_opt(const char *argv) {
	if (argv[0] == '-')
		return bootable_true;
	else
		return bootable_false;
}

/* * * * * * * * * * * *
 * Command Declarations
 * * * * * * * * * * * */

static int bootable_init(const char *config, const char *disk, int argc, const char **argv) {

	(void) argc;
	(void) argv;

	struct bootable_util util;

	bootable_util_init(&util);

	int err = bootable_util_open_config(&util, config);
	if (err != 0) {
		/* print message if it's not a syntax error */
		if (err != BOOTABLE_EINVAL)
			fprintf(stderr, "Failed to open config '%s': %s\n", config, bootable_strerror(err));
		bootable_util_done(&util);
		return EXIT_FAILURE;
	}

	err = bootable_util_create_disk(&util, disk);
	if (err != 0) {
		fprintf(stderr, "Failed to create disk image: %s\n", bootable_strerror(err));
		bootable_util_done(&util);
		return EXIT_FAILURE;
	}

	bootable_util_done(&util);

	return EXIT_SUCCESS;
}

static int bootable_ls(struct bootable_util *util, int argc, const char **argv) {

	struct bootable_fs *fs = &util->fs;

	if (argc == 0) {
		const char *default_args[] = { "/", NULL };
		return bootable_ls(util, 1,  default_args);
	}

	struct bootable_dir *subdir;

	for (int i = 0; i < argc; i++) {

		subdir = bootable_fs_open_dir(fs, argv[i]);
		if (subdir == NULL) {
			fprintf(stderr, "Failed to open '%s'.\n", argv[i]);
			return EXIT_FAILURE;
		}

		printf("%s:\n", argv[i]);

		for (bootable_uint64 j = 0; j < subdir->subdir_count; j++)
			printf("dir  : %s\n", subdir->subdirs[j].name);

		for (bootable_uint64 j = 0; j < subdir->file_count; j++)
			printf("file : %s\n", subdir->files[j].name);
	}

	return EXIT_SUCCESS;
}

static int bootable_mkdir(struct bootable_util *util, int argc, const char **argv) {

	struct bootable_fs *fs = &util->fs;

	for (int i = 0; i < argc; i++) {
		int err = bootable_fs_make_dir(fs, argv[i]);
		if (err != 0) {
			fprintf(stderr, "Failed to create directory '%s'.\n", argv[i]);
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

static int bootable_cat(struct bootable_util *util, int argc, const char **argv) {

	struct bootable_fs *fs = &util->fs;

	struct bootable_file *file;

	for (int i = 0; i < argc; i++) {

		file = bootable_fs_open_file(fs, argv[i]);
		if (file == NULL) {
			fprintf(stderr, "Failed to open '%s'.\n", argv[i]);
			return EXIT_FAILURE;
		}

		fwrite(file->data, 1, file->data_size, stdout);
	}

	return EXIT_SUCCESS;
}

static int bootable_cp(struct bootable_util *util, int argc, const char **argv) {

	if (argc <= 0) {
		fprintf(stderr, "Missing source path.\n");
		return EXIT_FAILURE;
	} else if (argc <= 1) {
		fprintf(stderr, "Missing destination path.\n");
		return EXIT_FAILURE;
	}

	const char *src_path = argv[0];
	const char *dst_path = argv[1];

	FILE *src = fopen(src_path, "rb");
	if (src == NULL) {
		fprintf(stderr, "Failed to open source file '%s'.\n", src_path);
		return EXIT_FAILURE;
	}

	int err = fseek(src, 0L, SEEK_END);

	long int src_size = ftell(src);

	err |= fseek(src, 0L, SEEK_SET);

	if ((err != 0) || (src_size < 0)) {
		fprintf(stderr, "Failed to get file size of '%s'.\n", src_path);
		fclose(src);
		return EXIT_FAILURE;
	}

	struct bootable_fs *fs = &util->fs;

	err = bootable_fs_make_file(fs, dst_path);
	if (err != 0) {
		fprintf(stderr, "Failed to create destination file '%s': %s.\n", dst_path, bootable_strerror(err));
		fclose(src);
		return EXIT_FAILURE;
	}

	struct bootable_file *dst;

	dst = bootable_fs_open_file(fs, dst_path);
	if (dst == NULL) {
		fprintf(stderr, "Failed to open destination file '%s'.\n", dst_path);
		fclose(src);
		return EXIT_FAILURE;
	}

	dst->data = malloc(src_size);
	if (dst->data == NULL) {
		fprintf(stderr, "Failed to allocate memory for destination file '%s'.\n", dst_path);
		fclose(src);
		return EXIT_FAILURE;
	}

	if (fread(dst->data, 1, src_size, src) != ((size_t) src_size)) {
		fprintf(stderr, "Failed to read source file'%s'.\n", src_path);
		fclose(src);
		return EXIT_FAILURE;
	}

	fclose(src);

	dst->data_size = src_size;

	return EXIT_SUCCESS;
}

int main(int argc, const char **argv) {

	const char *disk = "bootable.img";

	const char *config = "bootable-config.txt";

	int i = 1;

	while (i < argc) {
		if (check_opt(argv[i], "help", 'h')) {
			print_help(argv[0]);
			return EXIT_FAILURE;
		} else if (check_opt(argv[i], "disk", 'd')) {
			disk = argv[i + 1];
			i += 2;
		} else if (check_opt(argv[i], "config", 'c')) {
			config = argv[i + 1];
			i += 2;
		} else if (is_opt(argv[i])) {
			fprintf(stderr, "Unknown option '%s'.\n", argv[i]);
			return EXIT_FAILURE;
		} else {
			break;
		}
	}

	if (disk == NULL) {
		fprintf(stderr, "No disk specified after '--disk' or '-d' option.\n");
		return EXIT_FAILURE;
	}

	if (config == NULL) {
		fprintf(stderr, "No configuration file specified after '--config' or '-c' option.\n");
		return EXIT_FAILURE;
	}

	if (i >= argc) {
		fprintf(stderr, "No command specified (see '--help').\n");
		return EXIT_FAILURE;
	}

	argv = &argv[i];
	argc -= i;

	/* argv[0] should now point to a command. */

	const char *command = argv[0];

	argv = &argv[1];
	argc--;

	/* argv[0] is now the command arguments */

	if (strcmp(command, "init") == 0) {
		return bootable_init(config, disk, argc, argv);
	}

	struct bootable_util util;

	bootable_util_init(&util);

	int err = bootable_util_open_config(&util, config);
	if (err != 0) {
		fprintf(stderr, "Failed to open '%s': %s\n", config, bootable_strerror(err));
		bootable_util_done(&util);
		return EXIT_FAILURE;
	}

	err = bootable_util_open_disk(&util, disk);
	if (err != 0) {
		fprintf(stderr, "Failed to open '%s': %s\n", disk, bootable_strerror(err));
		bootable_util_done(&util);
		return EXIT_FAILURE;
	}

	int exit_code = EXIT_SUCCESS;

	if (strcmp(command, "cat") == 0) {
		exit_code = bootable_cat(&util, argc, argv);
	} else if (strcmp(command, "cp") == 0) {
		exit_code = bootable_cp(&util, argc, argv);
	} else if (strcmp(command, "ls") == 0) {
		exit_code = bootable_ls(&util, argc, argv);
	} else if (strcmp(command, "mkdir") == 0) {
		exit_code = bootable_mkdir(&util, argc, argv);
	} else {
		fprintf(stderr, "Unknown command '%s'.\n", command);
		bootable_util_done(&util);
		return EXIT_FAILURE;
	}

	if (exit_code != EXIT_SUCCESS) {
		bootable_util_done(&util);
		return EXIT_FAILURE;
	}

	err = bootable_util_save_disk(&util);
	if (err != 0) {
		fprintf(stderr, "Failed to save disk changes.\n");
		bootable_util_done(&util);
		return EXIT_FAILURE;
	}

	bootable_util_done(&util);

	return EXIT_SUCCESS;
}
