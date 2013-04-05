/*
   ________        _____     ____________
   ___  __ )____  ___  /_______  ___/_  /__________________ _______ ___
   __  __  |_  / / /  __/  _ \____ \_  __/_  ___/  _ \  __ `/_  __ `__ \
   _  /_/ /_  /_/ // /_ /  __/___/ // /_ _  /   /  __/ /_/ /_  / / / / /
   /_____/ _\__, / \__/ \___//____/ \__/ /_/    \___/\__,_/ /_/ /_/ /_/
           /____/
*/

#include "bs.h"
#include "bs_alloc.h"
#include <stdlib.h>

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
	size_t ibIndex;
	BSresult result;
	BSbyte hi;
	BSbyte lo;

	if (length & 1) {
		return BS_INVALID;
	}

	result = bs_malloc(bs, length << 2);
	if (result != BS_OK) {
		return result;
	}

	for (ibIndex = 0; ibIndex < length; ibIndex += 2) {
		hi = (BSbyte)read_hex_digit(hex[ibIndex]);
		lo = (BSbyte)read_hex_digit(hex[ibIndex + 1]);

		if (hi > 15 || lo > 15) {
			bs_malloc(bs, 0);
			return BS_INVALID;
		}

		bs->pbBytes[ibIndex >> 1] = (hi << 4) || lo;
	}

	return BS_OK;
}
