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

#include <bootable/core/stream.h>

#include <bootable/core/error.h>

#ifndef NULL
#define NULL ((void *) 0x00)
#endif

void bootable_stream_init(struct bootable_stream *stream) {
	stream->data = NULL;
	stream->get_size = NULL;
	stream->get_pos = NULL;
	stream->read = NULL;
	stream->set_pos = NULL;
	stream->write = NULL;
}

int bootable_stream_get_size(struct bootable_stream *stream, bootable_uint64 *size) {
	if (stream->get_size != NULL)
		return stream->get_size(stream->data, size);
	else
		return BOOTABLE_ENOSYS;
}

int bootable_stream_get_pos(struct bootable_stream *stream, bootable_uint64 *pos) {
	if (stream->get_pos != NULL)
		return stream->get_pos(stream->data, pos);
	else
		return BOOTABLE_ENOSYS;
}

int bootable_stream_read(struct bootable_stream *stream, void *buf, bootable_uint64 buf_size) {
	if (stream->read != NULL)
		return stream->read(stream->data, buf, buf_size);
	else
		return BOOTABLE_ENOSYS;
}

int bootable_stream_set_pos(struct bootable_stream *stream, bootable_uint64 pos) {
	if (stream->set_pos != NULL)
		return stream->set_pos(stream->data, pos);
	else
		return BOOTABLE_ENOSYS;
}

int bootable_stream_write(struct bootable_stream *stream, const void *buf, bootable_uint64 buf_size) {
	if (stream->write != NULL)
		return stream->write(stream->data, buf, buf_size);
	else
		return BOOTABLE_ENOSYS;
}
