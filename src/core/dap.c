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

#include <bootable/core/dap.h>

#include <bootable/core/stream.h>

#include "misc.h"

int bootable_dap_read(struct bootable_dap *dap, struct bootable_stream *stream) {

	int err = bootable_stream_read(stream, dap->reserved, sizeof(dap->reserved));
	if (err != 0)
		return err;

	err = decode_uint16(&dap->sector_count, stream);
	if (err != 0)
		return err;

	err = decode_uint16(&dap->address, stream);
	if (err != 0)
		return err;

	err = decode_uint16(&dap->segment, stream);
	if (err != 0)
		return err;

	err = decode_uint64(&dap->sector, stream);
	if (err != 0)
		return err;

	return 0;
}

int bootable_dap_write(const struct bootable_dap *dap, struct bootable_stream *stream) {

	int err = bootable_stream_write(stream, dap->reserved, sizeof(dap->reserved));
	if (err != 0)
		return err;

	err = encode_uint16(dap->sector_count, stream);
	if (err != 0)
		return err;

	err = encode_uint16(dap->address, stream);
	if (err != 0)
		return err;

	err = encode_uint16(dap->segment, stream);
	if (err != 0)
		return err;

	err = encode_uint64(dap->sector, stream);
	if (err != 0)
		return err;

	return 0;
}
