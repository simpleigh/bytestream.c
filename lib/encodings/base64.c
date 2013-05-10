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
#include "../bs_internal.h"
#include "../encodings.h"
#include <assert.h>

static const unsigned int
base64_decoding_table[] = {
	                                            62, 99, 99, 99, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 99, 99, 99, 77, 99, 99,
	99,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 99, 99, 99, 99, 99,
	99, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

static BSbyte
read_base64_digit(char digit)
{
	if ((digit < 43) || (digit > 122)) {
		return 99;
	}

	return base64_decoding_table[digit - 43];
}

static BSresult
read_base64_block(const char *in, BSbyte *out)
{
	BSbyte in0, in1, in2, in3;

	in0 = read_base64_digit(in[0]);
	in1 = read_base64_digit(in[1]);
	in2 = read_base64_digit(in[2]);
	in3 = read_base64_digit(in[3]);

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

BSresult
bs_decode_base64(BS *bs, const char *input, size_t length)
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
			bs->pbBytes + ibByteStream
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

size_t
bs_encode_size_base64(const BS *bs)
{
	return ((bs->cbBytes + 2) / 3 * 4) + 1;
}

static const char
base64_encoding_table[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void
write_base64_bytes(const BSbyte *in, size_t length, char *out)
{
	assert(length > 0);

	switch (length) {
	case 1:
		out[0] = base64_encoding_table[in[0] >> 2];
		out[1] = base64_encoding_table[(in[0] & 0x3) << 4];
		out[2] = '=';
		out[3] = '=';
		break;

	case 2:
		out[0] = base64_encoding_table[in[0] >> 2];
		out[1] = base64_encoding_table[(in[0] & 0x3) << 4 | in[1] >> 4];
		out[2] = base64_encoding_table[(in[1] & 0xF) << 2];
		out[3] = '=';
		break;

	default:
		out[0] = base64_encoding_table[in[0] >> 2];
		out[1] = base64_encoding_table[(in[0] & 0x3) << 4 | in[1] >> 4];
		out[2] = base64_encoding_table[(in[1] & 0xF) << 2 | in[2] >> 6];
		out[3] = base64_encoding_table[in[2] & 0x3F];
		break;
	}
}

void
bs_encode_base64(const BS *bs, char *output)
{
	size_t cbByteStream, ibOutput = 0, ibByteStream = 0;

	cbByteStream = bs->cbBytes;

	while (ibByteStream < cbByteStream) {
		write_base64_bytes(
			bs->pbBytes + ibByteStream,
			cbByteStream - ibByteStream,
			output + ibOutput
		);

		ibByteStream += 3;
		ibOutput += 4;
	}

	output[ibOutput] = '\0';
}
