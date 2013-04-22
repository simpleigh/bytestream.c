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
#include <string.h>

BSresult
bs_stream(
	BS *bs,
	const BSbyte *data,
	size_t length,
	BSresult (*operation) (const BS *bs)
)
{
	size_t cbRemaining = length;
	size_t cbSpace = bs->cbBytes - bs->cbStream;
	BSresult result;

	BS_ASSERT_VALID(bs)

	/* If we're already streaming */
	if (bs->cbStream > 0) {
		if (cbSpace > length) { /* Not enough bytes to fill buffer */
			memcpy(bs->pbBytes + bs->cbBytes, data, length);
			bs->cbStream += length;
			return BS_OK;
		} else { /* Fill up and execute */
			memcpy(bs->pbBytes + bs->cbBytes, data, cbSpace);
			cbRemaining -= cbSpace;
			bs->cbStream = 0;

			result = operation(bs);
			if (result != BS_OK) {
				return result;
			}
		}
	}

	/* Loop over each chunk */
	while (cbRemaining >= bs->cbBytes) {
		memcpy(bs->pbBytes, data, cbSpace);
		cbRemaining -= bs->cbBytes;

		result = operation(bs);
		if (result != BS_OK) {
			return result;
		}
	}

	/* Deal with any leftover data */
	if (cbRemaining > 0) {
		memcpy(bs->pbBytes, data, cbRemaining);
		bs->cbStream = cbRemaining;
	}

	return BS_OK;
}

BSresult
bs_flush_stream(BS *bs, BSresult (*operation) (const BS *bs))
{
	BS *bsOutput;

	if (bs->cbStream == 0) {
		return BS_OK;
	}

	bsOutput = bs_create_size(bs->cbStream);
	memcpy(bsOutput->pbBytes, bs->pbBytes, bs->cbStream);
	bs->cbStream = 0;

	return operation(bsOutput);
}

BSresult
bs_empty_stream(BS *bs, BSbyte **data, size_t *length)
{
	BSresult result;

	result = bs_malloc_output(
		bs->cbStream * sizeof(BSbyte),
		(void **) data,
		length
	);
	if (result != BS_OK) {
		return result;
	}

	memcpy(*data, bs->pbBytes, bs->cbStream);
	*length = bs->cbStream;
	bs->cbStream = 0;

	return BS_OK;
}
