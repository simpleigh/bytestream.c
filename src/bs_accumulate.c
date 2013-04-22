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
bs_accumulate(
	const BS *bs,
	BSresult (*operation) (BSbyte byte, void *data),
	void *data
)
{
	size_t ibByteStream;
	BSresult result;

	BS_CHECK_POINTER(bs)
	BS_ASSERT_VALID(bs)

	for (ibByteStream = 0; ibByteStream < bs->cbBytes; ibByteStream++) {
		result = operation(bs->pbBytes[ibByteStream], data);
		if (result != BS_OK) {
			return result;
		}
	}

	return BS_OK;
}

static BSresult
sum_byte(BSbyte byte, void *data)
{
	unsigned int *piSum = (unsigned int *) data;
	unsigned int iOriginalSum;

	assert(piSum != NULL);

	iOriginalSum = *piSum;

	*piSum += byte;

	if (*piSum < iOriginalSum) {
		return BS_OVERFLOW;
	}

	return BS_OK;
}

static BSresult
count_byte(BSbyte byte, void *data)
{
	unsigned int *piCount = (unsigned int *) data;
	unsigned int iOriginalCount;

	assert(piCount != NULL);

	iOriginalCount = *piCount;

	*piCount = *piCount
	         + (byte & 128) / 128
	         + (byte &  64) /  64
	         + (byte &  32) /  32
	         + (byte &  16) /  16
	         + (byte &   8) /   8
	         + (byte &   4) /   4
	         + (byte &   2) /   2
	         + (byte &   1) /   1;

	if (*piCount < iOriginalCount) {
		return BS_OVERFLOW;
	}

	return BS_OK;
}

BSresult
bs_accumulate_sum(const BS *bs, unsigned int *sum)
{
	BS_CHECK_POINTER(bs)
	BS_CHECK_POINTER(sum)

	*sum = 0;

	return bs_accumulate(bs, sum_byte, sum);
}

BSresult
bs_accumulate_bits(const BS *bs, unsigned int *count)
{
	BS_CHECK_POINTER(bs)
	BS_CHECK_POINTER(count)

	*count = 0;

	return bs_accumulate(bs, count_byte, count);
}
