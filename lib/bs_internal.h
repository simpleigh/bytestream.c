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

#ifndef __BS_ALLOC_H
#define __BS_ALLOC_H

#include "bs.h"
#include <assert.h>

struct BS {
	size_t cbBytes;  /* Current length of the byte stream */
	BSbyte *pbBytes; /* Byte stream buffer location */
	size_t cbBuffer; /* Size of the buffer */
	size_t cbStream; /* Count of queued bytes for a streaming operation */
};

/**
 * Check a pointer is non-null
 * Use in function headers to return BS_NULL for invalid input.
 */
#define BS_CHECK_POINTER(bs) if ((bs) == NULL) return BS_NULL;

/**
 * Check BS valid
 * Simple macro to check that a BS has been initialised correctly.
 */
#define BS_ASSERT_VALID(bs) \
	assert((bs) != NULL); \
	assert(((bs)->pbBytes != NULL) || ((bs)->cbBuffer == 0)); \
	assert((bs)->cbBytes <= (bs)->cbBuffer); \
	assert(((bs)->cbStream < (bs)->cbBytes) || ((bs)->cbBytes == 0));

/**
 * Mark unused function parameters
 */
#define UNUSED(x) (void)(x)

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
