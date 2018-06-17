/* =============================================================================
 * Pure64 -- a 64-bit OS/software loader written in Assembly for x86-64 systems
 * Copyright (C) 2008-2017 Return Infinity -- see LICENSE.TXT
 * =============================================================================
 */

#ifndef BOOTABLE_MISC_H
#define BOOTABLE_MISC_H

#include <bootable/core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bootable_stream;

int encode_uint16(bootable_uint16 n, struct bootable_stream *file);

int encode_uint32(bootable_uint32 n, struct bootable_stream *file);

int encode_uint64(bootable_uint64 n, struct bootable_stream *file);

int decode_uint16(bootable_uint16 *n_ptr, struct bootable_stream *file);

int decode_uint32(bootable_uint32 *n_ptr, struct bootable_stream *file);

int decode_uint64(bootable_uint64 *n_ptr, struct bootable_stream *file);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* BOOTABLE_MISC_H */
