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

BSresult
bs_walk(BS *bs, BSbyte (*operation) (BSbyte byte))
{
	size_t ibByteStream;

	BS_CHECK_POINTER(bs)
	BS_ASSERT_VALID(bs)

	for (ibByteStream = 0; ibByteStream < bs->cbBytes; ibByteStream++) {
		bs->pbBytes[ibByteStream] = operation(bs->pbBytes[ibByteStream]);
	}

	return BS_OK;
}

static BSbyte
uppercase_byte(BSbyte byte)
{
	return (byte >= 'a' && byte <= 'z') ? byte - 'a' + 'A' : byte;
}

static BSbyte
lowercase_byte(BSbyte byte)
{
	return (byte >= 'A' && byte <= 'Z') ? byte - 'A' + 'a' : byte;
}

static BSbyte
not_byte(BSbyte byte)
{
	return ~byte;
}

BSresult
bs_walk_uppercase(BS *bs)
{
	return bs_walk(bs, uppercase_byte);
}

BSresult
bs_walk_lowercase(BS *bs)
{
	return bs_walk(bs, lowercase_byte);
}

BSresult
bs_walk_not(BS *bs)
{
	return bs_walk(bs, not_byte);
}
