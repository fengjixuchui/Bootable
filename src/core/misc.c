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

#include "misc.h"

#include <bootable/core/stream.h>

int encode_uint16(bootable_uint16 n, struct bootable_stream *file) {

	int err;
	unsigned char buf[2];

	buf[0] = (n >> 0) & 0xff;
	buf[1] = (n >> 8) & 0xff;

	err = bootable_stream_write(file, buf, 2);
	if (err != 0)
		return err;

	return 0;
}

int encode_uint32(bootable_uint32 n, struct bootable_stream *file) {

	unsigned char buf[4];
	buf[0] = (n >> 0) & 0xff;
	buf[1] = (n >> 8) & 0xff;
	buf[2] = (n >> 16) & 0xff;
	buf[3] = (n >> 24) & 0xff;

	int err = bootable_stream_write(file, buf, 4);
	if (err != 0)
		return err;

	return 0;
}

int encode_uint64(bootable_uint64 n, struct bootable_stream *file) {

	int err;
	unsigned char buf[8];

	buf[0] = (n >> 0x00) & 0xff;
	buf[1] = (n >> 0x08) & 0xff;
	buf[2] = (n >> 0x10) & 0xff;
	buf[3] = (n >> 0x18) & 0xff;
	buf[4] = (n >> 0x20) & 0xff;
	buf[5] = (n >> 0x28) & 0xff;
	buf[6] = (n >> 0x30) & 0xff;
	buf[7] = (n >> 0x38) & 0xff;

	err = bootable_stream_write(file, buf, 8);
	if (err != 0)
		return err;

	return 0;
}

int decode_uint16(bootable_uint16 *n_ptr, struct bootable_stream *file) {

	unsigned char buf[2];

	int err = bootable_stream_read(file, buf, 2);
	if (err != 0)
		return err;

	bootable_uint16 n = 0;
	n |= ((bootable_uint16) buf[0]) << 0;
	n |= ((bootable_uint16) buf[1]) << 8;

	*n_ptr = n;

	return 0;
}

int decode_uint32(bootable_uint32 *n_ptr, struct bootable_stream *file) {

	unsigned char buf[4];

	int err = bootable_stream_read(file, buf, sizeof(buf));
	if (err != 0)
		return err;

	bootable_uint32 n = 0;
	n |= ((bootable_uint32) buf[0]) << 0;
	n |= ((bootable_uint32) buf[1]) << 8;
	n |= ((bootable_uint32) buf[2]) << 16;
	n |= ((bootable_uint32) buf[3]) << 24;

	*n_ptr = n;

	return 0;
}

int decode_uint64(bootable_uint64 *n_ptr, struct bootable_stream *file) {

	int err;
	unsigned char buf[8];

	err = bootable_stream_read(file, buf, 8);
	if (err != 0)
		return err;

	bootable_uint64 n = 0;
	n |= ((bootable_uint64) buf[0] << 0x00);
	n |= ((bootable_uint64) buf[1] << 0x08);
	n |= ((bootable_uint64) buf[2] << 0x10);
	n |= ((bootable_uint64) buf[3] << 0x18);
	n |= ((bootable_uint64) buf[4] << 0x20);
	n |= ((bootable_uint64) buf[5] << 0x28);
	n |= ((bootable_uint64) buf[6] << 0x30);
	n |= ((bootable_uint64) buf[7] << 0x38);

	*n_ptr = n;

	return 0;
}
