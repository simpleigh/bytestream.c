/*
   ________        _____     ____________
   ___  __ )____  ___  /_______  ___/_  /__________________ _______ ___
   __  __  |_  / / /  __/  _ \____ \_  __/_  ___/  _ \  __ `/_  __ `__ \
   _  /_/ /_  /_/ // /_ /  __/___/ // /_ _  /   /  __/ /_/ /_  / / / / /
   /_____/ _\__, / \__/ \___//____/ \__/ /_/    \___/\__,_/ /_/ /_/ /_/
           /____/
*/

#include "bs.h"
#include "bs_alloc.h"
#include <assert.h>
#include <stdlib.h>

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

	return bs;
}

BSresult
bs_malloc(BS *bs, size_t cbSize)
{
	BSbyte *pbNewBytes;

	assert((bs->pbBytes != NULL) || (bs->cbBuffer == 0));
	assert(bs->cbBytes <= bs->cbBuffer);

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
	return BS_OK;
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
	if ((bs->cbBuffer > 0) && (bs->pbBytes != NULL)) {
		free(bs->pbBytes);
	}
	free(bs);
}

size_t
bs_size(const BS *bs)
{
	return bs->cbBytes;
}
