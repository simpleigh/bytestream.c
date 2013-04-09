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

#ifndef __BS_H
#define __BS_H

#include <stddef.h>

/**
 * An individual byte
 * Bytes are stored unsigned, i.e. they may range from 0-255.
 */
typedef unsigned char BSbyte;

/**
 * The byte stream itself
 * The contents of the struct should be considered an implementation detail:
 * these may change without warning.
 */
typedef struct BS BS;

/**
 * Result ENUM
 * Each method call returns this type in order to indicate its status.
 * New error conditions may be added in future versions.
 */
typedef enum {
	BS_OK = 0,
	BS_INVALID, /* Invalid input data */
	BS_MEMORY   /* Memory allocation problem */
} BSresult;

/**
 * Create a byte stream
 * Creates an empty byte stream and returns a pointer to it.
 * Returns NULL if memory cannot be allocated.
 */
BS *bs_create(void);

/**
 * Create a byte stream with a particular size
 * Creates a byte stream with the specified length and returns a pointer to it.
 * Returns NULL if memory cannot be allocated.
 * Byte stream data is left uninitialised and shouldn't be used.
 */
BS *bs_create_size(size_t length);

/*
 * Free a byte stream
 * Frees all memory used by a byte stream.
 * Once a stream pointer has been freed then it should not be reused.
 */
void bs_free(BS *bs);

/**
 * Calculate the length of a byte stream
 * Returns the number of bytes held in a byte stream.
 */
size_t bs_size(const BS *bs);

/*
 * Zero a byte stream
 * Sets all bytes in a byte stream to zero.
 */
void bs_zero(BS *bs);

/**
 * Get a byte
 * Returns an individual byte from a byte stream.
 * Streams are zero-indexed, and indices must be within [ 0, bs_size(bs) ).
 * An out-of-range index will lead to undefined behaviour.
 */
BSbyte bs_byte_get(const BS *bs, size_t index);

/**
 * Set a byte
 * Sets an individual byte in a byte stream and returns that byte.
 * Streams are zero-indexed, and indices must be within [ 0, bs_size(bs) ).
 * An out-of-range index will lead to undefined behaviour.
 */
BSbyte bs_byte_set(BS *bs, size_t index, BSbyte byte);

/**
 * Load binary data
 * Reads binary data into the byte stream.
 * Returns BS_OK if data is loaded correctly
 * Returns BS_MEMORY and leaves the byte stream untouched for memory issues
 */
BSresult bs_load_binary(BS *bs, const unsigned char *data, size_t length);

/**
 * Load a string
 * Reads a string into the byte stream.
 * The string must only contain character values within [ 0, 127 ].
 * Returns BS_OK if the string is loaded correctly
 * Returns BS_MEMORY and leaves the byte stream untouched for memory issues
 * Returns BS_INVALID and empties the byte stream for out-of-range input
 */
BSresult bs_load_string(BS *bs, const char *string, size_t length);

/**
 * Load a hexadecimal string
 * Reads a hexadecimal string into the byte stream.
 * Each pair of hex digits is read as a single byte.
 * Hex digits may be in upper or lowercase.
 * Returns BS_OK if the string is loaded correctly
 * Returns BS_MEMORY and leaves the byte stream untouched for memory issues
 * Returns BS_INVALID if the input length is not even
 * Returns BS_INVALID and empties the byte stream for invalid hexadecimal digits
 */
BSresult bs_load_hex(BS *bs, const char *hex, size_t length);

/**
 * Load a base64-encoded string
 * Reads a base64-encoded string.
 * Returns BS_OK if the string is loaded correctly
 * Returns BS_MEMORY and leaves the byte stream untouched for memory issues
 * Returns BS_INVALID if the input length is not a multiple of four
 * Returns BS_INVALID and empties the byte stream for invalid input data
 */
BSresult bs_load_base64(BS *bs, const char *base64, size_t length);

#endif /* __BS_H */
