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

#include <bootable/core/error.h>

const char *bootable_strerror(int err) {

	switch (err) {
	case 0:
		/* Zero indicates no error */
		break;
	case BOOTABLE_EFAULT:
		return "Bad address was detected.";
	case BOOTABLE_ENOMEM:
		return "Not enough memory available.";
	case BOOTABLE_EISDIR:
		return "Entry is a directory.";
	case BOOTABLE_ENOTDIR:
		return "Entry is not a directory.";
	case BOOTABLE_ENOENT:
		return "No such file or directory.";
	case BOOTABLE_EEXIST:
		return "Entry exists already.";
	case BOOTABLE_EINVAL:
		return "Invalid argument was detected.";
	case BOOTABLE_ENOSYS:
		return "Functionality not implemented.";
	case BOOTABLE_EIO:
		return "I/O error occured";
	}

	return "Success";
}
