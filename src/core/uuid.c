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

#include <bootable/core/uuid.h>

#include <bootable/core/string.h>

static char bootable_tolower(char c) {

	if ((c >= 'A') && (c <= 'Z'))
		c = 'a' + (c - 'A');

	return c;
}

void bootable_uuid_zero(struct bootable_uuid *uuid) {
	bootable_memset(uuid->bytes, 0, sizeof(uuid->bytes));
}

void bootable_uuid_copy(struct bootable_uuid *dst,
                      const struct bootable_uuid *src) {
	bootable_memcpy(dst->bytes, src->bytes, sizeof(dst->bytes));
}

int bootable_uuid_cmp(const struct bootable_uuid *a,
                    const struct bootable_uuid *b) {

	return bootable_memcmp(a->bytes, b->bytes, sizeof(a->bytes));
}

int bootable_uuid_parse(struct bootable_uuid *uuid, const char *str) {

	unsigned int i;
	unsigned int j;

	i = 0;
	j = 0;

	while ((i < 16) && (str[j] != 0)) {

		uuid->bytes[i] = 0;

		char c = bootable_tolower(str[j]);
		if ((c >= '0') && (c <= '9'))
			uuid->bytes[i] |= ((c - '0') + 0x00) << 4;
		else if ((c >= 'a') && (c <= 'f'))
			uuid->bytes[i] |= ((c - 'a') + 0x0a) << 4;
		else if (c == '-') {
			j++;
			continue;
		} else
			return -1;

		c = bootable_tolower(str[j + 1]);
		if ((c >= '0') && (c <= '9'))
			uuid->bytes[i] |= (c - '0') + 0x00;
		else if ((c >= 'a') && (c <= 'f'))
			uuid->bytes[i] |= (c - 'a') + 0x0a;
		else
			return -1;

		j += 2;
		i++;
	}

	/* adjust for little endian encoding */

	unsigned char buf[16];

	/* little endian */

	buf[0] = uuid->bytes[3];
	buf[1] = uuid->bytes[2];
	buf[2] = uuid->bytes[1];
	buf[3] = uuid->bytes[0];

	/* little endian */

	buf[4] = uuid->bytes[5];
	buf[5] = uuid->bytes[4];

	/* little endian */

	buf[6] = uuid->bytes[7];
	buf[7] = uuid->bytes[6];

	/* big endian */

	buf[8] = uuid->bytes[8];
	buf[9] = uuid->bytes[9];

	/* big endian */

	buf[10] = uuid->bytes[10];
	buf[11] = uuid->bytes[11];
	buf[12] = uuid->bytes[12];
	buf[13] = uuid->bytes[13];
	buf[14] = uuid->bytes[14];
	buf[15] = uuid->bytes[15];

	for (i = 0; i < 16; i++)
		uuid->bytes[i] = buf[i];

	return 0;
}
