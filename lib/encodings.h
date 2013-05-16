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

#ifndef __ENCODINGS_H
#define __ENCODINGS_H

#include "bs.h"

struct BSencoding {
	char *szName;
	BSresult (*fpDecode) (BS *bs, const char *input, size_t length);
	size_t (*fpSize) (const BS *bs);
	void (*fpEncode) (const BS *bs, char *output);
};

BSresult bs_decode_hex       (BS *bs, const char *input, size_t length);
BSresult bs_decode_base64    (BS *bs, const char *input, size_t length);
BSresult bs_decode_base64url (BS *bs, const char *input, size_t length);

size_t bs_encode_size_hex    (const BS *bs);
size_t bs_encode_size_base64 (const BS *bs);

void bs_encode_hex       (const BS *bs, char *hex);
void bs_encode_base64    (const BS *bs, char *hex);
void bs_encode_base64url (const BS *bs, char *hex);

#endif /* __ENCODINGS_H */
