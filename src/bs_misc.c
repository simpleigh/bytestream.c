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

static BSbyte
hamming_callback(BSbyte byte1, BSbyte byte2, void *data)
{
	unsigned int *pDistance = (unsigned int *) data;

	*pDistance = *pDistance
	           + ((byte1 ^ byte2) & 128) / 128
	           + ((byte1 ^ byte2) &  64) /  64
	           + ((byte1 ^ byte2) &  32) /  32
	           + ((byte1 ^ byte2) &  16) /  16
	           + ((byte1 ^ byte2) &   8) /   8
	           + ((byte1 ^ byte2) &   4) /   4
	           + ((byte1 ^ byte2) &   2) /   2
	           + ((byte1 ^ byte2) &   1) /   1;

	return byte1; /* Byte stream left unchanged */
}

BSresult bs_hamming(BS *bs1, const BS *bs2, unsigned int *distance)
{
	if (bs1->cbBytes != bs2->cbBytes) {
		return BS_INVALID;
	}

	*distance = 0;
	return bs_combine(bs1, bs2, hamming_callback, (void *) distance);
}
