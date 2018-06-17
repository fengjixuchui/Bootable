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

#ifndef BOOTABLE_LANG_SYNTAX_ERROR_H
#define BOOTABLE_LANG_SYNTAX_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

/** A syntax error, caused by
 * a mistake in the configuration file.
 * @ingroup lang-api
 * */

struct bootable_syntax_error {
	/** The name of the source that
	 * the error originated from. This
	 * is usually a filename or @ref bootable_null. */
	const char *source;
	/** A description of the error that
	 * occurred. */
	const char *desc;
	/** The line that the error was found at. */
	unsigned long int line;
	/** The column that the error was found at. */
	unsigned long int column;
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BOOTABLE_LANG_SYNTAX_ERROR_H */
