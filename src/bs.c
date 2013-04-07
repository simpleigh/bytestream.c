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

void
bs_zero(BS *bs)
{
	size_t ibIndex;

	if (bs_size(bs) == 0) {
		return;
	}

	assert(bs->pbBytes != NULL);

	for (ibIndex = 0; ibIndex < bs_size(bs); ibIndex++) {
		bs->pbBytes[ibIndex] = 0;
	}
}

BSbyte
bs_byte_get(const BS *bs, size_t index)
{
	assert(index < bs_size(bs));
	assert(bs->pbBytes != NULL);

	return bs->pbBytes[index];
}

BSbyte
bs_byte_set(BS *bs, size_t index, BSbyte byte)
{
	assert(index < bs_size(bs));
	assert(bs->pbBytes != NULL);

	return bs->pbBytes[index] = byte;
}
