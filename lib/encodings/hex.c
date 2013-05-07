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

	BS_CHECK_POINTER(bs)
	BS_CHECK_POINTER(hex)

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

size_t
bs_size_hex(const BS *bs)
{
	BS_ASSERT_VALID(bs)

	return (2 * bs->cbBytes) + 1;
}

static const char
hex_encoding_table[] = "0123456789abcdef";

BSresult
bs_save_hex(const BS *bs, char *hex)
{
	size_t ibStream;
	BSbyte bByte;

	BS_CHECK_POINTER(bs)
	BS_CHECK_POINTER(hex)
	BS_ASSERT_VALID(bs)

	for (ibStream = 0; ibStream < bs->cbBytes; ibStream++) {
		bByte = bs->pbBytes[ibStream];
		hex[2 * ibStream]     = hex_encoding_table[bByte >> 4];
		hex[2 * ibStream + 1] = hex_encoding_table[bByte & 0xF];
	}

	hex[2 * ibStream] = '\0';

	return BS_OK;
}
