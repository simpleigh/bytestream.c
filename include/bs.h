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
typedef enum BSresult {
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
BSbyte bs_get_byte(const BS *bs, size_t index);

/**
 * Set a byte
 * Sets an individual byte in a byte stream and returns that byte.
 * Streams are zero-indexed, and indices must be within [ 0, bs_size(bs) ).
 * An out-of-range index will lead to undefined behaviour.
 */
BSbyte bs_set_byte(BS *bs, size_t index, BSbyte byte);

/**
 * Load binary data
 * Reads binary data into the byte stream.
 * Returns BS_OK if data is loaded correctly
 * Returns BS_MEMORY if memory cannot be allocated
 * Do not attempt to use the bytestream if the return value is other than BS_OK.
 */
BSresult bs_load_binary(BS *bs, const unsigned char *data, size_t length);

/**
 * Save binary data
 * Writes the byte stream as binary data.
 * Space for the binary data will be allocated, and should be freed when no
 * longer required.
 * Returns BS_OK if data is saved correctly
 * Returns BS_MEMORY if memory cannot be allocated
 * The bytestream is not touched by this operation.
 */
BSresult bs_save_binary(const BS *bs, unsigned char **data, size_t *length);

/**
 * Load a string
 * Reads a string into the byte stream.
 * The string must only contain character values within [ 0, 127 ].
 * Pass the length of the string excluding the terminating null byte ('\0').
 * Returns BS_OK if the string is loaded correctly
 * Returns BS_MEMORY if memory cannot be allocated
 * Returns BS_INVALID if an input character is not valid
 * Do not attempt to use the bytestream if the return value is other than BS_OK.
 */
BSresult bs_load_string(BS *bs, const char *string, size_t length);

/**
 * Save a string
 * Writes the byte stream as a string.
 * Space for the string will be allocated, and should be freed when no longer
 * required.
 * Provides the length of the string excluding the terminating null byte ('\0').
 * Returns BS_OK if data is saved correctly
 * Returns BS_MEMORY if memory cannot be allocated
 * Returns BS_INVALID if a byte cannot safely be rendered as as string
 * The bytestream is not touched by this operation.
 */
BSresult bs_save_string(const BS *bs, char **string, size_t *length);

/**
 * Load a hexadecimal string
 * Reads a hexadecimal string into the byte stream.
 * Each pair of hex digits is read as a single byte.
 * Hex digits may be in upper or lowercase.
 * Returns BS_OK if the string is loaded correctly
 * Returns BS_MEMORY if memory cannot be allocated
 * Returns BS_INVALID if the input length is not even
 * Returns BS_INVALID if an input character is not valid
 * Do not attempt to use the bytestream if the return value is other than BS_OK.
 */
BSresult bs_load_hex(BS *bs, const char *hex, size_t length);

/**
 * Save a hexadecimal string
 * Writes the byte stream as a hexadecimal string.
 * Space for the string will be allocated, and should be freed when no longer
 * required.
 * Provides the length of the string excluding the terminating null byte ('\0').
 * Returns BS_OK if data is saved correctly
 * Returns BS_MEMORY if memory cannot be allocated
 * The bytestream is not touched by this operation.
 */
BSresult bs_save_hex(const BS *bs, char **hex, size_t *length);

/**
 * Load a base64-encoded string
 * Reads a base64-encoded string.
 * Returns BS_OK if the string is loaded correctly
 * Returns BS_MEMORY if memory cannot be allocated
 * Returns BS_INVALID if the input length is not a multiple of four
 * Returns BS_INVALID if an input character is not valid
 * Do not attempt to use the bytestream if the return value is other than BS_OK.
 */
BSresult bs_load_base64(BS *bs, const char *base64, size_t length);

/**
 * Save a base64-encoded string
 * Writes the byte stream as a base-64 encoded string.
 * Space for the string will be allocated, and should be freed when no longer
 * required.
 * Provides the length of the string excluding the terminating null byte ('\0').
 * Returns BS_OK if data is saved correctly
 * Returns BS_MEMORY if memory cannot be allocated
 * The bytestream is not touched by this operation.
 */
BSresult bs_save_base64(const BS *bs, char **base64, size_t *length);

/**
 * Filters a byte stream
 * Applies an OPERATION to each byte in a byte stream.
 * Common operation are available through library functions defined below.
 * Returns BS_OK if all bytes are filtered successfully
 * The operation should return the filtered value.
 */
BSresult bs_filter(BS *bs, BSbyte (*operation) (BSbyte byte));

/**
 * Make characters uppercase
 */
BSresult bs_filter_uppercase(BS *bs);

/**
 * Make characters lowercase
 */
BSresult bs_filter_lowercase(BS *bs);

/**
 * NOT each byte
 */
BSresult bs_filter_not(BS *bs);

/**
 * Combine two byte streams
 * Applies an operand byte stream based on an operation. OPERAND is duplicated
 * or truncated to match the length of the input BS. Its bytes are then combined
 * with the input BS using the supplied OPERATION.
 * This provides a simple way, for example, to XOR two streams together.
 * Common operations are available through library functions defined below.
 * Returns BS_OK if all bytes are combined successfully
 * The operation should return the combination of BYTE1 and BYTE2.
 */
BSresult bs_combine(BS *bs, const BS *operand,
	BSbyte (*operation) (BSbyte byte1, BSbyte byte2));

/**
 * XOR two byte streams
 */
BSresult bs_combine_xor(BS *bs, const BS *operand);

/**
 * OR two byte streams
 */
BSresult bs_combine_or(BS *bs, const BS *operand);

/**
 * AND two byte streams
 */
BSresult bs_combine_and(BS *bs, const BS *operand);

/**
 * Add two byte streams
 * Addition overflows naturally (i.e. 255 + 1 = 0).
 */
BSresult bs_combine_add(BS *bs, const BS *operand);

/**
 * Subtract two byte streams
 * Subtraction overflows naturally (i.e. 0 - 1 = 255).
 */
BSresult bs_combine_sub(BS *bs, const BS *operand);

#endif /* __BS_H */
