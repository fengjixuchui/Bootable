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

/** @file error.h API related to error codes. */

#ifndef BOOTABLE_ERROR_H
#define BOOTABLE_ERROR_H

/** A bad address was passed to a function. */

#define BOOTABLE_EFAULT  0x01

/** No more memory available. */

#define BOOTABLE_ENOMEM  0x02

/** The entry is a directory (and it shouldn't be). */

#define BOOTABLE_EISDIR  0x03

/** The entry is not a directory (but it should be). */

#define BOOTABLE_ENOTDIR 0x04

/** No such file or directory. */

#define BOOTABLE_ENOENT  0x05

/** The file or directory already exists. */

#define BOOTABLE_EEXIST  0x06

/** Invalid argument */

#define BOOTABLE_EINVAL  0x07

/** Function not implemented. */

#define BOOTABLE_ENOSYS  0x08

/** Input/Output error occured. */

#define BOOTABLE_EIO 0x09

/** Not enough storage space for
 * the operation to complete. */

#define BOOTABLE_ENOSPC 0x0a

#ifdef __cplusplus
extern "C" {
#endif

/** Get a string representation of an
 * error that occured in the Pure64 library.
 * @param err The error code that was returned.
 * @returns A human-readable error description.
 * */

const char *bootable_strerror(int err);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_ERROR_H */
