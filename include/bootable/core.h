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

#ifndef BOOTABLE_CORE_H
#define BOOTABLE_CORE_H

/** @defgroup core-api Core API
 * The library common to both the
 * bootloader and utility program.
 * */

#include <bootable/core/arch.h>
#include <bootable/core/dap.h>
#include <bootable/core/dir.h>
#include <bootable/core/e820.h>
#include <bootable/core/error.h>
#include <bootable/core/file.h>
#include <bootable/core/fs.h>
#include <bootable/core/gpt.h>
#include <bootable/core/mbr.h>
#include <bootable/core/memory.h>
#include <bootable/core/partition.h>
#include <bootable/core/path.h>
#include <bootable/core/stream.h>
#include <bootable/core/string.h>
#include <bootable/core/types.h>
#include <bootable/core/uuid.h>

#endif /* BOOTABLE_CORE_H */
