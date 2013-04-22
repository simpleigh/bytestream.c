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
#include "bs_alloc.h"
#include <assert.h>

BSresult
bs_load(BS *bs, const BSbyte *data, size_t length)
{
	size_t ibData;
	BSresult result;

	result = bs_malloc(bs, length);
	if (result != BS_OK) {
		return result;
	}

	for (ibData = 0; ibData < length; ibData++) {
		bs->pbBytes[ibData] = data[ibData];
	}

	return BS_OK;
}

BSresult
bs_save(const BS *bs, BSbyte **data, size_t *length)
{
	size_t ibStream;
	BSresult result;

	BS_ASSERT_VALID(bs);

	result = bs_malloc_output(
		bs->cbBytes * sizeof(**data),
		(void **) data,
		length
	);
	if (result != BS_OK) {
		return result;
	}

	for (ibStream = 0; ibStream < bs->cbBytes; ibStream++) {
		(*data)[ibStream] = bs->pbBytes[ibStream];
	}

	return BS_OK;
}

static BSbyte
read_hex_digit(char digit)
{
	if ((digit >= '0' && digit <= '9')) {
		return digit - '0';
	}

	if ((digit >= 'a' && digit <= 'f')) {
		return digit - 'a' + 0xA;
	}

	if ((digit >= 'A' && digit <= 'F')) {
		return digit - 'A' + 0xA;
	}

	return 16;
}

BSresult
bs_load_hex(BS *bs, const char *hex, size_t length)
{
	size_t ibHex;
	BSresult result;
	BSbyte hi;
	BSbyte lo;

	if (length & 1) {
		return BS_INVALID;
	}

	result = bs_malloc(bs, length >> 1);
	if (result != BS_OK) {
		return result;
	}

	for (ibHex = 0; ibHex < length; ibHex += 2) {
		hi = (BSbyte)read_hex_digit(hex[ibHex]);
		lo = (BSbyte)read_hex_digit(hex[ibHex + 1]);

		if (hi > 15 || lo > 15) {
			bs_malloc(bs, 0);
			return BS_INVALID;
		}

		bs->pbBytes[ibHex >> 1] = (hi << 4) | lo;
	}

	return BS_OK;
}

static const char
hex_encoding_table[] = "0123456789abcdef";

BSresult
bs_save_hex(const BS *bs, char **hex, size_t *length)
{
	size_t ibStream;
	BSresult result;
	BSbyte bByte;

	BS_ASSERT_VALID(bs)

	result = bs_malloc_output(
		2 * bs->cbBytes * sizeof(**hex),
		(void **) hex,
		length
	);
	if (result != BS_OK) {
		return result;
	}

	for (ibStream = 0; ibStream < bs->cbBytes; ibStream++) {
		bByte = bs->pbBytes[ibStream];
		(*hex)[2 * ibStream]     = hex_encoding_table[bByte >> 4];
		(*hex)[2 * ibStream + 1] = hex_encoding_table[bByte & 0xF];
	}

	return BS_OK;
}

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

	out[1] = '\0';
	out[2] = '\0';
	out[3] = '\0';

	out[0] = in0 << 2 | in1 >> 4;
	if (in[2] != '=') {
		out[1] = (in1 & 0xF) << 4 | in2 >> 2;
		if (in[3] != '=')
			out[2] = (in2 & 0x3) << 6 | in3;
	}

	return BS_OK;
}

BSresult
bs_load_base64(BS *bs, const char *base64, size_t length)
{
	size_t cbByteStream, ibBase64 = 0, ibByteStream = 0;
	BSresult result;

	if (length & 3) {
		return BS_INVALID;
	}

	cbByteStream = (length >> 2) * 3;
	if (base64[length - 1] == '=') {
		cbByteStream--;
		if (base64[length - 2] == '=') {
			cbByteStream--;
		}
	}

	result = bs_malloc(bs, cbByteStream);
	if (result != BS_OK) {
		return result;
	}

	while (ibBase64 < length) {
		result = read_base64_block(
			base64 + ibBase64,
			bs->pbBytes + ibByteStream
		);

		if (result != BS_OK) {
			bs_malloc(bs, 0);
			return result;
		}

		ibByteStream += 3;
		ibBase64 += 4;
	}

	return BS_OK;
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

BSresult
bs_save_base64(const BS *bs, char **base64, size_t *length)
{
	size_t cbByteStream = bs->cbBytes, ibBase64 = 0, ibByteStream = 0;
	BSresult result;

	BS_ASSERT_VALID(bs)

	result = bs_malloc_output(
		((cbByteStream + 2) / 3 * 4) * sizeof(**base64),
		(void **) base64,
		length
	);
	if (result != BS_OK) {
		return result;
	}

	while (ibByteStream < cbByteStream) {
		write_base64_bytes(
			bs->pbBytes + ibByteStream,
			cbByteStream - ibByteStream,
			*base64 + ibBase64
		);

		ibByteStream += 3;
		ibBase64 += 4;
	}

	return BS_OK;
}
