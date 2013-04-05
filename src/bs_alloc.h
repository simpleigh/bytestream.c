/*
   ________        _____     ____________
   ___  __ )____  ___  /_______  ___/_  /__________________ _______ ___
   __  __  |_  / / /  __/  _ \____ \_  __/_  ___/  _ \  __ `/_  __ `__ \
   _  /_/ /_  /_/ // /_ /  __/___/ // /_ _  /   /  __/ /_/ /_  / / / / /
   /_____/ _\__, / \__/ \___//____/ \__/ /_/    \___/\__,_/ /_/ /_/ /_/
           /____/
*/

#ifndef __BS_ALLOC_H
#define __BS_ALLOC_H

#include "bs.h"

struct BS {
	size_t cbBytes;
	BSbyte *pbBytes;
	size_t cbBuffer;
};

/**
 * Allocate internal memory
 * Ensures that the byte stream's internal buffer is at least CBSIZE bytes long,
 * and sets the internal size to that value.
 * If the buffer isn't large enough already then realloc() will be called to
 * enlarge it.
 * Returns BS_OK if the buffer can be made large enough
 * Returns BS_MEMORY and leaves the byte stream untouched for memory issues
 */
BSresult bs_malloc(BS *bs, size_t cbSize);

#endif /* __BS_ALLOC_H */
