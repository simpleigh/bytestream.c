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

void
bs_zero(BS *bs)
{
	size_t ibIndex;

	BS_ASSERT_VALID(bs)

	if (bs_size(bs) == 0) {
		return;
	}

	assert(bs->pbBytes != NULL);

	for (ibIndex = 0; ibIndex < bs_size(bs); ibIndex++) {
		bs->pbBytes[ibIndex] = 0;
	}
}

BSbyte
bs_get_byte(const BS *bs, size_t index)
{
	BS_ASSERT_VALID(bs)
	assert(index < bs_size(bs));

	return bs->pbBytes[index];
}

BSbyte
bs_set_byte(BS *bs, size_t index, BSbyte byte)
{
	BS_ASSERT_VALID(bs)
	assert(index < bs_size(bs));

	return bs->pbBytes[index] = byte;
}

BSresult
bs_load(BS *bs, const BSbyte *data, size_t length)
{
	size_t ibData;
	BSresult result;

	result = bs_malloc(bs, length);
	if (result != BS_OK) {
		return result;
	}

	for (ibData = 0; ibData < length; ibData++) {
		bs->pbBytes[ibData] = data[ibData];
	}

	return BS_OK;
}

BSresult
bs_save(const BS *bs, BSbyte **data, size_t *length)
{
	size_t ibStream;
	BSresult result;

	BS_ASSERT_VALID(bs);

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
