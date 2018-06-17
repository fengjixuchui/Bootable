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

/** @file stream.h API related to stream functions. */

#ifndef BOOTABLE_STREAM_H
#define BOOTABLE_STREAM_H

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A Pure64 stream.
 * */

struct bootable_stream {
	/** Implementation data */
	void *data;
	/** Get size callback */
	int (*get_size)(void *data, bootable_uint64 *size);
	/** Get position callback */
	int (*get_pos)(void *data, bootable_uint64 *offset);
	/** Read callback */
	int (*read)(void *data, void *buf, bootable_uint64 buf_size);
	/** Set position callback */
	int (*set_pos)(void *data, bootable_uint64 offset);
	/** Write callback */
	int (*write)(void *data, const void *buf, bootable_uint64 buf_size);
};

/** Initializes the stream for use.
 * @param stream An uninitialized stream structure.
 * */

void bootable_stream_init(struct bootable_stream *stream);

/** Get the size of the stream. If
 * the stream is resizeable, then it is
 * the span of the stream that actually
 * contains data.
 * @param stream An initialized stream structure.
 * @param size The pointer to the variable that
 * will receive the size of the stream.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_stream_get_size(struct bootable_stream *stream, bootable_uint64 *size);

/** Gets the current position of the stream.
 * @param stream An initialized stream structure.
 * @param pos A pointer to the variable that will
 * receive the current position of the stream.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_stream_get_pos(struct bootable_stream *stream, bootable_uint64 *pos);

/** Reads data from the stream.
 * @param stream An initialized stream structure.
 * @param buf The buffer to put the data in.
 * @param buf_size The number of bytes to read and
 * the size of the buffer.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_stream_read(struct bootable_stream *stream, void *buf, bootable_uint64 buf_size);

/** Sets the position of the stream.
 * @param stream An initialized stream structure.
 * @param pos The position to set the stream to.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_stream_set_pos(struct bootable_stream *stream, bootable_uint64 pos);

/** Writes data to the stream.
 * @param stream An initialized stream structure.
 * @param buf The buffer containing the data.
 * @param buf_size The number of bytes in the buffer.
 * @returns Zero on success, non-zero on failure.
 * */

int bootable_stream_write(struct bootable_stream *stream, const void *buf, bootable_uint64 buf_size);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_STREAM_H */
