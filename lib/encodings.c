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

BSresult
bs_decode(BS *bs, const char *encoding, const char *input, size_t length)
{
	BS_CHECK_POINTER(encoding)

	if (strcmp(encoding, "base64") == 0) {
		return bs_decode_base64(bs, input, length);
	} else if (strcmp(encoding, "hex") == 0) {
		return bs_decode_hex(bs, input, length);
	} else {
		return BS_BAD_ENCODING;
	}
}

size_t
bs_encode_size(const BS *bs, const char *encoding)
{
	BS_CHECK_POINTER(encoding)

	if (strcmp(encoding, "base64") == 0) {
		return bs_encode_size_base64(bs);
	} else if (strcmp(encoding, "hex") == 0) {
		return bs_encode_size_hex(bs);
	} else {
		return BS_BAD_ENCODING;
	}
}

BSresult
bs_encode(const BS *bs, const char *encoding, char *output)
{
	BS_CHECK_POINTER(encoding)

	if (strcmp(encoding, "base64") == 0) {
		return bs_encode_base64(bs, output);
	} else if (strcmp(encoding, "hex") == 0) {
		return bs_encode_hex(bs, output);
	} else {
		return BS_BAD_ENCODING;
	}
}
