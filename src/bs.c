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
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void
bs_zero(BS *bs)
{
	assert(bs != NULL);
	BS_ASSERT_VALID(bs)

	memset(bs->pbBytes, 0, bs->cbBytes);
}

BSbyte
bs_get_byte(const BS *bs, size_t index)
{
	assert(bs != NULL);
	BS_ASSERT_VALID(bs)
	assert(index < bs->cbBytes);

	return bs->pbBytes[index];
}

BSbyte
bs_set_byte(BS *bs, size_t index, BSbyte byte)
{
	assert(bs != NULL);
	BS_ASSERT_VALID(bs)
	assert(index < bs->cbBytes);

	return bs->pbBytes[index] = byte;
}

BSresult
bs_load(BS *bs, const BSbyte *data, size_t length)
{
	BSresult result;

	if (length == 0) {
		return BS_OK;
	}

	BS_CHECK_POINTER(bs)
	BS_CHECK_POINTER(data)

	result = bs_malloc(bs, length);
	if (result != BS_OK) {
		return result;
	}

	memcpy(bs->pbBytes, data, length);

	return BS_OK;
}

BSresult
bs_save(const BS *bs, BSbyte **data, size_t *length)
{
	BSresult result;

	BS_CHECK_POINTER(bs)
	BS_ASSERT_VALID(bs)

	result = bs_malloc_output(
		bs->cbBytes * sizeof(**data),
		(void **) data,
		length
	);
	if (result != BS_OK) {
		return result;
	}

	memcpy(*data, bs->pbBytes, bs->cbBytes);

	return BS_OK;
}
