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

#include "libbs.h"
#include "bs_internal.h"

BSresult
bs_compare(
	const BS *bs1,
	const BS *bs2,
	BSresult (*operation) (BSbyte byte1, BSbyte byte2, void *data),
	void *data
)
{
	size_t ibByteStream = 0;
	BSresult result;

	BS_CHECK_POINTER(bs1)
	BS_CHECK_POINTER(bs2)
	BS_CHECK_POINTER(operation)
	BS_ASSERT_VALID(bs1)
	BS_ASSERT_VALID(bs2)

	if (bs1->cbBytes != bs2->cbBytes) {
		return BS_INVALID;
	}

	for (ibByteStream = 0; ibByteStream < bs1->cbBytes; ibByteStream++) {
		result = operation(
			bs1->pbBytes[ibByteStream],
			bs2->pbBytes[ibByteStream],
			data
		);

		if (result != BS_OK) {
			return result;
		}
	}

	return BS_OK;
}

static BSresult
compare_equal(BSbyte byte1, BSbyte byte2, void *data)
{
	UNUSED(data);

	if (byte1 == byte2) {
		return BS_OK;
	} else {
		return BS_INVALID;
	}
}

static BSresult
compare_hamming(BSbyte byte1, BSbyte byte2, void *data)
{
	unsigned int *piDistance = (unsigned int *) data;
	unsigned int iOriginalDistance = *piDistance;

	*piDistance = *piDistance
	           + ((byte1 ^ byte2) & 128) / 128
	           + ((byte1 ^ byte2) &  64) /  64
	           + ((byte1 ^ byte2) &  32) /  32
	           + ((byte1 ^ byte2) &  16) /  16
	           + ((byte1 ^ byte2) &   8) /   8
	           + ((byte1 ^ byte2) &   4) /   4
	           + ((byte1 ^ byte2) &   2) /   2
	           + ((byte1 ^ byte2) &   1) /   1;

	if (*piDistance < iOriginalDistance) {
		return BS_OVERFLOW;
	}

	return BS_OK;
}

BSresult bs_compare_equal(const BS *bs1, const BS *bs2)
{
	return bs_compare(bs1, bs2, compare_equal, NULL);
}

BSresult bs_compare_hamming(const BS *bs1, const BS *bs2, unsigned int *distance)
{
	BS_CHECK_POINTER(distance)

	*distance = 0;

	return bs_compare(bs1, bs2, compare_hamming, distance);
}
