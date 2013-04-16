/*
   ________        _____     ____________
   ___  __ )____  ___  /_______  ___/_  /__________________ _______ ___
   __  __  |_  / / /  __/  _ \____ \_  __/_  ___/  _ \  __ `/_  __ `__ \
   _  /_/ /_  /_/ // /_ /  __/___/ // /_ _  /   /  __/ /_/ /_  / / / / /
   /_____/ _\__, / \__/ \___//____/ \__/ /_/    \___/\__,_/ /_/ /_/ /_/
           /____/

   Byte stream manipulation library.
   Copyright (C) 2013  Leigh Simpson <code@simpleigh.com>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or any
   later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   A copy of the GNU Lesser General Public License is available within
   COPYING.LGPL; alternatively write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "bs.h"
#include "bs_alloc.h"
#include <stdlib.h>

BSresult
bs_load_binary(BS *bs, const unsigned char *data, size_t length)
{
	size_t ibStream;
	BSresult result;

	result = bs_malloc(bs, length);
	if (result != BS_OK) {
		return result;
	}

	for (ibStream = 0; ibStream < length; ibStream++) {
		bs->pbBytes[ibStream] = data[ibStream];
	}

	return BS_OK;
}

BSresult
bs_save_binary(const BS *bs, unsigned char **data, size_t *length)
{
	size_t ibStream;
	BSresult result;

	BS_ASSERT_VALID(bs)

	result = bs_malloc_output(
		bs_size(bs) * sizeof(**data),
		(void **) data,
		length
	);
	if (result != BS_OK) {
		return result;
	}

	for (ibStream = 0; ibStream < bs_size(bs); ibStream++) {
		(*data)[ibStream] = bs->pbBytes[ibStream];
	}

	return BS_OK;
}

BSresult
bs_load_string(BS *bs, const char *string, size_t length)
{
	size_t ibStream;
	BSresult result;

	result = bs_malloc(bs, length);
	if (result != BS_OK) {
		return result;
	}

	for (ibStream = 0; ibStream < length; ibStream++) {
		if (string[ibStream] < 0) { /* Negative values not allowed */
			bs_malloc(bs, 0);
			return BS_INVALID;
		}
		bs->pbBytes[ibStream] = (BSbyte)string[ibStream];
	}

	return BS_OK;
}

BSresult
bs_save_string(const BS *bs, char **string, size_t *length)
{
	size_t ibStream;
	BSresult result;
	char c;

	BS_ASSERT_VALID(bs)

	result = bs_malloc_output(
		bs_size(bs) * sizeof(**string),
		(void **) string,
		length
	);
	if (result != BS_OK) {
		return result;
	}

	for (ibStream = 0; ibStream < bs_size(bs); ibStream++) {
		c = bs->pbBytes[ibStream];
		if ((c < 32) || (c > 126)) {
			free(*string);
			*string = NULL;
			*length = 0;
			return BS_INVALID;
		}
		(*string)[ibStream] = c;
	}

	return BS_OK;
}
