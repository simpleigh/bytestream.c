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
#include "bs_internal.h"
#include "encodings.h"
#include <string.h>

static const struct BSencoding rgEncodings[] = {
	{ "base64", bs_decode_base64, bs_encode_size_base64, bs_encode_base64 },
	{ "hex",    bs_decode_hex,    bs_encode_size_hex,    bs_encode_hex    },
	{ NULL,     NULL,             NULL,                  NULL             },
};

BSresult
bs_decode(BS *bs, const char *encoding, const char *input, size_t length)
{
	size_t iEncoding = 0;

	BS_CHECK_POINTER(bs)
	BS_CHECK_POINTER(encoding)
	BS_CHECK_POINTER(input)

	while (rgEncodings[iEncoding].szName != NULL) {
		if (strcmp(encoding, rgEncodings[iEncoding].szName) == 0) {
			return rgEncodings[iEncoding].fpDecode(bs, input, length);
		}
		iEncoding++;
	}

	return BS_BAD_ENCODING;
}

size_t
bs_encode_size(const BS *bs, const char *encoding)
{
	size_t iEncoding = 0;

	BS_ASSERT_VALID(bs)
	BS_CHECK_POINTER(encoding)

	while (rgEncodings[iEncoding].szName != NULL) {
		if (strcmp(encoding, rgEncodings[iEncoding].szName) == 0) {
			return rgEncodings[iEncoding].fpSize(bs);
		}
		iEncoding++;
	}

	return 0;
}

BSresult
bs_encode(const BS *bs, const char *encoding, char *output)
{
	size_t iEncoding = 0;

	BS_CHECK_POINTER(bs)
	BS_ASSERT_VALID(bs)
	BS_CHECK_POINTER(encoding)
	BS_CHECK_POINTER(output)

	while (rgEncodings[iEncoding].szName != NULL) {
		if (strcmp(encoding, rgEncodings[iEncoding].szName) == 0) {
			rgEncodings[iEncoding].fpEncode(bs, output);
			return BS_OK;
		}
		iEncoding++;
	}

	return BS_BAD_ENCODING;
}
