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

#include "libbs.h"
#include "../bs_internal.h"
#include "../encodings.h"
#include <assert.h>


/* ====== */
/* Decode */
/* ====== */

/**
 * These tables are used to convert an ASCII character to its base64 value
 *  99 = invalid character
 *  77 = padding ('=')
 */
static const unsigned int
rgBase64Decoding[] = {
/*       0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
/* 2 */                                              62, 99, 99, 99, 63,
/* 3 */  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 99, 99, 99, 77, 99, 99,
/* 4 */  99,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
/* 5 */  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 99, 99, 99, 99, 99,
/* 6 */  99, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
/* 7 */  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

static const unsigned int
rgBase64UrlDecoding[] = {
/*       0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
/* 2 */                                              99, 99, 62, 99, 99,
/* 3 */  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 99, 99, 99, 77, 99, 99,
/* 4 */  99,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
/* 5 */  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 99, 99, 99, 99, 63,
/* 6 */  99, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
/* 7 */  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

static BSbyte
read_base64_digit(char digit, const unsigned int rgDecoding[])
{
	if ((digit < 43) || (digit > 122)) {
		return 99;
	}

	return rgDecoding[digit - 43];
}

static BSresult
read_base64_block(const char *in, BSbyte *out, const unsigned int rgDecoding[])
{
	BSbyte in0, in1, in2, in3;

	in0 = read_base64_digit(in[0], rgDecoding);
	in1 = read_base64_digit(in[1], rgDecoding);
	in2 = read_base64_digit(in[2], rgDecoding);
	in3 = read_base64_digit(in[3], rgDecoding);

	if ((in0 == 99) || (in1 == 99) || (in2 == 99) || (in3 == 99)) {
		return BS_INVALID;
	}

	out[0] = in0 << 2 | in1 >> 4;
	if (in[2] != '=') {
		out[1] = (in1 & 0xF) << 4 | in2 >> 2;
		if (in[3] != '=')
			out[2] = (in2 & 0x3) << 6 | in3;
	}

	return BS_OK;
}

static BSresult
read_base64_string(
	BS *bs,
	const char *input,
	size_t length,
	const unsigned int rgDecoding[]
)
{
	size_t cbByteStream, ibInput = 0, ibByteStream = 0;
	BSresult result;

	if (length & 3) {
		return BS_INVALID;
	}

	cbByteStream = (length >> 2) * 3;
	if (input[length - 1] == '=') {
		cbByteStream--;
		if (input[length - 2] == '=') {
			cbByteStream--;
		}
	}

	result = bs_malloc(bs, cbByteStream);
	if (result != BS_OK) {
		return result;
	}

	while (ibInput < length) {
		result = read_base64_block(
			input + ibInput,
			bs->pbBytes + ibByteStream,
			rgDecoding
		);

		if (result != BS_OK) {
			bs_malloc(bs, 0);
			return result;
		}

		ibByteStream += 3;
		ibInput += 4;
	}

	return BS_OK;
}

BSresult
bs_decode_base64(BS *bs, const char *input, size_t length)
{
	return read_base64_string(bs, input, length, rgBase64Decoding);
}

BSresult
bs_decode_base64url(BS *bs, const char *input, size_t length)
{
	return read_base64_string(bs, input, length, rgBase64UrlDecoding);
}


/* ==== */
/* Size */
/* ==== */

size_t
bs_encode_size_base64(const BS *bs)
{
	return ((bs->cbBytes + 2) / 3 * 4) + 1;
}


/* ====== */
/* Encode */
/* ====== */

static const char
rgBase64Encoding[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char
rgBase64UrlEncoding[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static void
write_base64_bytes(
	const BSbyte *in,
	size_t length,
	char *out,
	const char rgEncoding[]
)
{
	assert(length > 0);

	switch (length) {
	case 1:
		out[0] = rgEncoding[in[0] >> 2];
		out[1] = rgEncoding[(in[0] & 0x3) << 4];
		out[2] = '=';
		out[3] = '=';
		break;

	case 2:
		out[0] = rgEncoding[in[0] >> 2];
		out[1] = rgEncoding[(in[0] & 0x3) << 4 | in[1] >> 4];
		out[2] = rgEncoding[(in[1] & 0xF) << 2];
		out[3] = '=';
		break;

	default:
		out[0] = rgEncoding[in[0] >> 2];
		out[1] = rgEncoding[(in[0] & 0x3) << 4 | in[1] >> 4];
		out[2] = rgEncoding[(in[1] & 0xF) << 2 | in[2] >> 6];
		out[3] = rgEncoding[in[2] & 0x3F];
		break;
	}
}

static void
write_base64_string(const BS *bs, char *output, const char rgEncoding[])
{
	size_t cbByteStream, ibOutput = 0, ibByteStream = 0;

	cbByteStream = bs->cbBytes;

	while (ibByteStream < cbByteStream) {
		write_base64_bytes(
			bs->pbBytes + ibByteStream,
			cbByteStream - ibByteStream,
			output + ibOutput,
			rgEncoding
		);

		ibByteStream += 3;
		ibOutput += 4;
	}

	output[ibOutput] = '\0';
}

void
bs_encode_base64(const BS *bs, char *output)
{
	write_base64_string(bs, output, rgBase64Encoding);
}

void
bs_encode_base64url(const BS *bs, char *output)
{
	write_base64_string(bs, output, rgBase64UrlEncoding);
}
