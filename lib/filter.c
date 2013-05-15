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

BSresult
bs_filter(BS *bs, BSfilter (*operation) (BSbyte byte))
{
	size_t ibRead = 0, ibWrite = 0;
	BSbyte bCurrent;

	BS_CHECK_POINTER(bs)
	BS_ASSERT_VALID(bs)
	BS_CHECK_POINTER(operation)

	while (ibRead < bs_size(bs)) {
		bCurrent = bs->pbBytes[ibRead];
		if (operation(bCurrent) == BS_INCLUDE) {
			bs->pbBytes[ibWrite] = bCurrent;
			ibWrite++;
		}
		ibRead++;
	}

	/* ibWrite now refers to last byte written */
	bs->cbBytes = ibWrite;

	return BS_OK;
}

BSfilter
filter_whitespace(BSbyte byte)
{
	switch (byte) {
		case 0x09: /* HT */
		case 0x0A: /* LF */
		case 0x0D: /* CR */
		case ' ':
			return BS_EXCLUDE;

		default:
			return BS_INCLUDE;
	}
}

BSresult
bs_filter_whitespace(BS *bs)
{
	return bs_filter(bs, filter_whitespace);
}
