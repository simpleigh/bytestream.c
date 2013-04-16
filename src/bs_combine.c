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
bs_combine(BS *bs, const BS *operand,
	BSbyte (*operation) (BSbyte byte1, BSbyte byte2))
{
	size_t cbByteStream = bs_size(bs), cbOperand = bs_size(operand);
	size_t ibByteStream = 0, ibOperand = 0;

	BS_ASSERT_VALID(bs);
	BS_ASSERT_VALID(operand);

	while (ibByteStream < cbByteStream) {
		bs->pbBytes[ibByteStream] = operation(
			bs->pbBytes[ibByteStream],
			operand->pbBytes[ibOperand]
		);

		ibByteStream++;
		ibOperand++;
		if (ibOperand == cbOperand) {
			ibOperand = 0;
		}
	}

	return BS_OK;
}

static BSbyte
xor_byte(BSbyte byte1, BSbyte byte2)
{
	return byte1 ^ byte2;
}

static BSbyte
or_byte(BSbyte byte1, BSbyte byte2)
{
	return byte1 | byte2;
}

static BSbyte
and_byte(BSbyte byte1, BSbyte byte2)
{
	return byte1 & byte2;
}

static BSbyte
add_byte(BSbyte byte1, BSbyte byte2)
{
	return byte1 + byte2;
}

static BSbyte
sub_byte(BSbyte byte1, BSbyte byte2)
{
	return byte1 - byte2;
}

BSresult
bs_combine_xor(BS *bs, const BS *operand)
{
	return bs_combine(bs, operand, xor_byte);
}

BSresult
bs_combine_or(BS *bs, const BS *operand)
{
	return bs_combine(bs, operand, or_byte);
}

BSresult
bs_combine_and(BS *bs, const BS *operand)
{
	return bs_combine(bs, operand, and_byte);
}

BSresult
bs_combine_add(BS *bs, const BS *operand)
{
	return bs_combine(bs, operand, add_byte);
}

BSresult
bs_combine_sub(BS *bs, const BS *operand)
{
	return bs_combine(bs, operand, sub_byte);
}
