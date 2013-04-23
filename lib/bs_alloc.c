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


/* **************** */
/* * EXTERNAL API * */
/* **************** */

BS *
bs_create(void)
{
	BS *bs;

	bs = malloc(sizeof(struct BS));
	if (bs == NULL) {
		return NULL;
	}

	bs->cbBytes = 0;
	bs->pbBytes = NULL;
	bs->cbBuffer = 0;
	bs->cbStream = 0;

	return bs;
}

BS *
bs_create_size(size_t length)
{
	BS *bs;
	BSresult result;

	bs = bs_create();
	if (bs == NULL) {
		return bs;
	}

	if (length > 0) {
		result = bs_malloc(bs, length);
		if (result != BS_OK) {
			bs_free(bs);
			return NULL;
		}
	}

	return bs;
}

void
bs_free(BS *bs)
{
	assert(bs != NULL);

	if ((bs->cbBuffer > 0) && (bs->pbBytes != NULL)) {
		free(bs->pbBytes);
	}
	free(bs);
}

size_t
bs_size(const BS *bs)
{
	assert(bs != NULL);

	return bs->cbBytes;
}

BSresult
bs_set_buffer(BS *bs, void *buffer, size_t length)
{
	BS_CHECK_POINTER(bs)
	BS_CHECK_POINTER(buffer)

	if (length == 0) {
		return BS_INVALID;
	}

	if ((bs->cbBuffer > 0) && (bs->pbBytes != NULL)) {
		free(bs->pbBytes);
	}

	bs->cbBytes = length;
	bs->pbBytes = buffer;
	bs->cbBuffer = length;
	bs->cbStream = 0;

	return BS_OK;
}


/* **************** */
/* * INTERNAL API * */
/* **************** */

BSresult
bs_malloc(BS *bs, size_t cbSize)
{
	BSbyte *pbNewBytes;

	BS_CHECK_POINTER(bs)
	BS_ASSERT_VALID(bs)

	if (cbSize <= bs->cbBuffer) { /* Buffer already long enough */
		bs->cbBytes = cbSize;
		return BS_OK;
	}

	pbNewBytes = realloc(bs->pbBytes, cbSize * sizeof(*(bs->pbBytes)));
	if (pbNewBytes == NULL) {
		return BS_MEMORY;
	}

	bs->cbBytes = cbSize;
	bs->pbBytes = pbNewBytes;
	bs->cbBuffer = cbSize;
	bs->cbStream = 0;

	return BS_OK;
}

BSresult
bs_malloc_output(size_t cbBytes, void **ppbOutput, size_t *pcbOutput)
{
	BS_CHECK_POINTER(ppbOutput);
	BS_CHECK_POINTER(pcbOutput);

	*pcbOutput = cbBytes;

	if (cbBytes == 0) {
		*ppbOutput = NULL;
		return BS_OK;
	}

	*ppbOutput = malloc(cbBytes + 1);
	if (*ppbOutput == NULL) {
		*pcbOutput = 0;
		return BS_MEMORY;
	}
	((char *) *ppbOutput)[cbBytes] = '\0'; /* Guarantee null-termination */

	return BS_OK;
}
