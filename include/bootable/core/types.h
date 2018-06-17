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

/** @file types.h
 * Common types and values used throughout the library.
 * */

#ifndef BOOTABLE_TYPES_H
#define BOOTABLE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/** A boolean true value.
 * @ingroup core-api
 * */

#define bootable_true 1

/** A boolean false  value.
 * @ingroup core-api
 * */

#define bootable_false 0

/** An invalid pointer value.
 * @ingroup core-api
 * */

#define bootable_null ((void *) 0x00)

/** The largest type available in the system.
 * @ingroup core-api
 * */

typedef unsigned long long int bootable_size;

/** A 64-bit unsigned integer.
 * @ingroup core-api
 * */

typedef unsigned long long int bootable_uint64;

/** A 32-bit unsigned integer.
 * @ingroup core-api
 * */

typedef unsigned int bootable_uint32;

/** A 16-bit unsigned integer.
 * @ingroup core-api
 * */

typedef unsigned short int bootable_uint16;

/** An 8-bit unsigned integer.
 * @ingroup core-api
 * */

typedef unsigned char bootable_uint8;

/** A boolean type.
 * Use @ref bootable_false and @ref bootable_true
 * for setting this value.
 * @ingroup core-api
 * */

typedef unsigned char bootable_bool;

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_TYPES_H */
