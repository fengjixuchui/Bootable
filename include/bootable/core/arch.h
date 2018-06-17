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

#ifndef BOOTABLE_ARCH_H
#define BOOTABLE_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

/** Enumerates all of
 * the supported architectures.
 * */

enum bootable_arch {
	/** This means no architecture
	 * was specified. */
	BOOTABLE_ARCH_none,
	/** This means the architecture is x86_64. */
	BOOTABLE_ARCH_x86_64
};

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_ARCH_H */
