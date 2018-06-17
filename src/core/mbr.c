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

#include <bootable/core/mbr.h>

#include <bootable/core/error.h>
#include <bootable/core/string.h>
#include <bootable/core/stream.h>

#include "misc.h"

void bootable_mbr_zero(struct bootable_mbr *mbr) {
	bootable_memset(mbr, 0, sizeof(*mbr));
}

int bootable_mbr_check(const struct bootable_mbr *mbr) {

	if (mbr->boot_signature != 0xaa55)
		return BOOTABLE_EINVAL;

	return 0;
}

int bootable_mbr_read(struct bootable_mbr *mbr, struct bootable_stream *stream) {

	int err;

	err = bootable_stream_set_pos(stream, 0x00);
	if (err != 0)
		return err;

	err = bootable_stream_read(stream, mbr->code, sizeof(mbr->code));
	if (err != 0)
		return err;

	err = bootable_dap_read(&mbr->st2dap, stream);
	if (err != 0)
		return err;

	err = bootable_dap_read(&mbr->st3dap, stream);
	if (err != 0)
		return err;

	err = decode_uint16(&mbr->boot_signature, stream);
	if (err != 0)
		return err;

	return 0;
}

int bootable_mbr_write(const struct bootable_mbr *mbr, struct bootable_stream *stream) {

	int err;

	err = bootable_stream_set_pos(stream, 0x00);
	if (err != 0)
		return err;

	err = bootable_stream_write(stream, mbr->code, sizeof(mbr->code));
	if (err != 0)
		return err;

	err = bootable_dap_write(&mbr->st2dap, stream);
	if (err != 0)
		return err;

	err = bootable_dap_write(&mbr->st3dap, stream);
	if (err != 0)
		return err;

	err = encode_uint16(mbr->boot_signature, stream);
	if (err != 0)
		return err;

	return 0;
}
