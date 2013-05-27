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

#ifdef __cplusplus
extern "C" {
#endif

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
	BS_INVALID,      /* Invalid input data */
	BS_NULL,         /* NULL pointer passed as input */
	BS_MEMORY,       /* Memory allocation problem */
	BS_OVERFLOW,     /* Integer overflow */
	BS_BAD_ENCODING  /* Unknown encoding scheme */
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

/**
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

/**
 * Get the internal buffer
 * Returns a pointer to the internal buffer held by the byte stream.
 * Returns NULL if the byte stream doesn't have a buffer allocated.
 * The buffer remains under the control of the byte stream:
 * you should not perform any memory operations on it (e.g. realloc / free).
 */
BSbyte *bs_get_buffer(const BS *bs);

/**
 * Set the internal buffer
 * Sets the internal buffer of bytes to point to a new location.
 * Bytes are stored within an internal buffer, and loading data usually involves
 * looping over input to copy bytes across. This method simply sets the
 * bytestream to point at a new memory location and is therefore much quicker.
 * Any previous buffer held by the stream will be freed.
 * The buffer remains under the control of the byte stream:
 * you should not perform any memory operations on it (e.g. realloc / free).
 */
BSresult bs_set_buffer(BS *bs, void *buffer, size_t length);

/**
 * Remove the internal buffer
 * Resets the internal buffer to NULL, forgetting about its contents.
 * This is intended for use with the bs_set_buffer, allowing an attached buffer
 * to be detached cleanly from the byte stream.
 */
void bs_unset_buffer(BS *bs);

/**
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
 * Load data
 * Reads data into the byte stream.
 * Returns BS_OK if data is loaded correctly
 * Returns BS_MEMORY if memory cannot be allocated
 * Do not attempt to use the bytestream if the return value is other than BS_OK.
 */
BSresult bs_load(BS *bs, const BSbyte *data, size_t length);

/**
 * Save data
 * Writes out data from the byte stream.
 * Returns BS_OK if data is saved correctly
 * DATA must be at least bs_size() bytes long.
 * The bytestream is not touched by this operation.
 */
BSresult bs_save(const BS *bs, BSbyte *data);

/**
 * Process a stream of data
 * Reads STREAM, calling OPERATION each time the byte stream becomes full.
 * STREAM may be longer or shorter than the byte stream:
 *  - if it's longer then the operation will be called multiple times
 *  - if it's shorter then the data will be queued until the stream is full
 * If used correctly then a bursts of input easily can be processed in chunks.
 * The DATA pointer can be used to persist status between calls.
 * Returns BS_OK if data has been read and processed correctly
 * Returns failure code from the underlying operation if errors occur
 */
BSresult bs_stream(
	BS *bs,
	const BSbyte *stream,
	size_t length,
	BSresult (*operation) (const BS *bs, void *data),
	void *data
);

/**
 * Flush out streamed bytes
 * If any unprocessed bytes are left in the stream then this will empty it,
 * calling the supplied operation to process them.
 * Returns BS_OK if data is saved correctly, or no bytes are queued
 * Returns failure code from the underlying operation if errors occur
 */
BSresult bs_stream_flush(
	BS *bs,
	BSresult (*operation) (const BS *bs, void *data),
	void *data
);

/**
 * Clear stream state
 * Resets the internal streaming state.
 * Returns BS_OK if data is saved correctly
 */
void bs_stream_reset(BS *bs);

/**
 * Load an encoded string
 * Reads an encoded string into the byte stream.
 * Supported encodings currently include:
 *  - base64
 *  - hex
 * Returns BS_OK if the string is loaded correctly
 * Returns BS_MEMORY if memory cannot be allocated
 * Returns BS_INVALID if the input cannot be decoded, e.g. if the length is
 * invalid or a character is out-of-range
 * Returns BS_BAD_ENCODING if the specified encoding is not known
 * Do not attempt to use the bytestream if the return value is other than BS_OK.
 */
BSresult bs_decode(
	BS *bs,
	const char *encoding,
	const char *input,
	size_t length
);

/**
 * Size an encoded string
 * Passes the size of the buffer required to write the byte stream as a string
 * encoded with the specified ENCODING.
 * The size includes space for a terminating null '\0' byte.
 * Returns BS_OK
 * Returns BS_BAD_ENCODING if the specified encoding is not known
 */
BSresult bs_encode_size(const BS *bs, const char *encoding, size_t *size);

/**
 * Save an encoded string
 * Writes the byte stream as a string with the specified ENCODING.
 * Returns BS_OK if data is saved correctly
 * The supplied buffer must be long enough to store the data.
 * The bytestream is not touched by this operation.
 * Returns BS_OK if the string is written correctly
 * Returns BS_BAD_ENCODING if the specified encoding is not known
 */
BSresult bs_encode(const BS *bs, const char *encoding, char *output);

/**
 * Map a byte stream
 * Applies an OPERATION to each byte in a byte stream.
 * Common operation are available through library functions defined below.
 * Returns BS_OK if all bytes are processed successfully
 * The OPERATION should return the new value for each byte.
 */
BSresult bs_map(BS *bs, BSbyte (*operation) (BSbyte byte));

/**
 * Make characters uppercase
 */
BSresult bs_map_uppercase(BS *bs);

/**
 * Make characters lowercase
 */
BSresult bs_map_lowercase(BS *bs);

/**
 * NOT each byte
 */
BSresult bs_map_not(BS *bs);

/**
 * Filter operation result ENUM
 * Filtering operations return values to indicate whether a byte should be
 * retained or removed from the stream.
 */
typedef enum BSfilter {
	BS_INCLUDE = 0,
	BS_EXCLUDE
} BSfilter;

/**
 * Filter out unwanted bytes from a byte stream
 * Applies OPERATION to each byte in the byte stream, removing the byte if the
 * result is false.
 * Common operations are available through library functions defined below.
 * Returns BS_OK if all bytes are read successfully
 */
BSresult bs_filter(BS *bs, BSfilter (*operation) (BSbyte byte));

/**
 * Remove whitespace from a bytes stream
 * Removes ' ', HT, CR and LF characters from the byte stream.
 */
BSresult bs_filter_whitespace(BS *bs);

/**
 * Collect a single value from a byte stream
 * Applies OPERATION to the byte stream, passing each byte in turn.
 * A pointer to DATA is passed to the operation so that it can maintain its
 * count between calls.
 * Common operations are available through library functions defined below.
 * Returns BS_OK if all bytes are read successfully
 */
BSresult bs_fold(
	const BS *bs,
	BSresult (*operation) (BSbyte byte, void *data),
	void *data
);

/**
 * Add a byte stream
 * Adds all bytes together.
 * Returns BS_OVERFLOW if the sum becomes too large
 */
BSresult bs_fold_sum(const BS *bs, unsigned int *sum);

/**
 * Count bits in a stream
 * Counts all bits which are set in the stream.
 * Returns BS_OVERFLOW if the count becomes too large
 */
BSresult bs_fold_bitcount(const BS *bs, unsigned int *count);

/**
 * Compare two byte streams
 * Applies OPERATION to two byte streams, passing in a byte from each.
 * A pointer to DATA is passed to the operation so that it can maintain its
 * count between calls.
 * Common operations are available through library functions defined below.
 * Returns BS_OK if all bytes are compared successfully
 * Returns BS_INVALID if the streams differ in length
 */
BSresult bs_compare(
	const BS *bs1,
	const BS *bs2,
	BSresult (*operation) (BSbyte byte1, BSbyte byte2, void *data),
	void *data
);

/**
 * Compare two streams
 * This abuses the return value in order to improve efficiency:
 * Returns BS_OK if the streams are equal
 * Returns BS_INVALID if they differ
 */
BSresult bs_compare_equal(const BS *bs1, const BS *bs2);

/**
 * Calculate the Hamming distance between two byte streams
 * Returns BS_OVERFLOW if the distance becomes too large
 */
BSresult bs_compare_hamming(const BS *bs1, const BS *bs2, unsigned int *distance);

#endif /* __BS_H */

/**
 * Combine two byte streams
 * Applies an operand byte stream based on an operation. OPERAND is duplicated
 * or truncated to match the length of the input BS. Its bytes are then combined
 * with the input BS using the supplied OPERATION.
 * This provides a simple way, for example, to XOR two streams together.
 * Common operations are available through library functions defined below.
 * Returns BS_OK if all bytes are combined successfully
 * Returns BS_INVALID for zero-length operand
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

#ifdef __cplusplus
}
#endif
