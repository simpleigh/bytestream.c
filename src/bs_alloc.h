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
	size_t cbBytes;
	BSbyte *pbBytes;
	size_t cbBuffer;
};

/**
 * Check BS valid
 * Simple macro to check that a BS has been initialised correctly.
 * Implemented as a macro so it can be optimised away with NDEBUG.
 */
#define BS_ASSERT_VALID(bs) \
	assert(((bs)->pbBytes != NULL) || ((bs)->cbBuffer == 0)); \
	assert((bs)->cbBytes <= (bs)->cbBuffer);

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

/**
 * Allocate external memory
 * Allocates memory for external data output and sets up output variables.
 * Attempts to allocate CBBYTES bytes and writes allocation data to pointers:
 *  - ppbOutput : pointer to variable which takes the buffer location pointer
 *  - pcbOutput : pointer to variable which takes the buffer size
 * If zero bytes are requested (unlikely as that may be), no memory will be
 * allocated, and NULL and 0 respectively saved to the above values.
 * In other cases the allocated buffer will be one byte larger than requested.
 * The extra byte will be set to '\0' to ensure that any string output is
 * correctly null-terminated; functions writing out strings don't need to
 * do this for themselves.
 * Returns BS_OK if memory was allocated correctly
 * Returns BS_MEMORY for memory issues and saves NULL and 0 respectively
 */
BSresult bs_malloc_output(size_t cbBytes, void **ppbOutput, size_t *pcbOutput);

#endif /* __BS_ALLOC_H */
