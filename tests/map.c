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
#include <check.h>
#include <stdlib.h>


/* =============================== */
/* Tiny functions used for testing */
/* =============================== */

static BSbyte
noop_byte(BSbyte byte)
{
	return byte;
}

static BSbyte
increment_byte(BSbyte byte)
{
	return byte + 1;
}

static BSresult
map_increment(BS *bs)
{
	return bs_map(bs, increment_byte);
}

static BSresult
map_noop(BS *bs)
{
	return bs_map(bs, noop_byte);
}


/* ========= */
/* Testcases */
/* ========= */

#define C_MAPS 5

static BSresult (*rgfMaps[C_MAPS])(BS *) = {
	map_noop,
	map_increment,
	bs_map_uppercase,
	bs_map_lowercase,
	bs_map_not
};

struct BSMapTestcase {
	BSresult (*pfMap)(BS *); /* Function to test */
	BSbyte *rgbInput;        /* Starting bytestream contents */
	size_t cbInput;          /* Starting bytestream length */
	const BSbyte *rgbOutput; /* Expected bytestream contents */
};

static struct BSMapTestcase
rgTestcases[] = {
	{ map_noop,         "\0\x7F\xFF",            3, "\0\x7F\xFF"           },
	{ map_noop,         "@[`{+,AZaz09",         12, "@[`{+,AZaz09"         },
	{ map_increment,    "00000",                 5, "11111"                },
	{ map_increment,    "11111",                 5, "22222"                },
	{ bs_map_uppercase, "\0\x7F\xFF",            3, "\0\x7F\xFF"           },
	{ bs_map_uppercase, "@[`{+,AZaz09",         12, "@[`{+,AZAZ09"         },
	{ bs_map_lowercase, "\0\x7F\xFF",            3, "\0\x7F\xFF"           },
	{ bs_map_lowercase, "@[`{+,AZaz09",         12, "@[`{+,azaz09"         },
	{ bs_map_not,       "\x01\x23\x45\x67\x89",  5, "\xFE\xDC\xBA\x98\x76" },
	{ bs_map_not,       "\xAB\xCD\xEF",          3, "\x54\x32\x10"         },
};


/* ============== */
/* Testcase tests */
/* ============== */

START_TEST(test_maps)
{
	struct BSMapTestcase testcase = rgTestcases[_i];
	BS *bs = bs_create();
	BSresult result;

	result = bs_load(bs, testcase.rgbInput, testcase.cbInput);
	fail_unless(result == BS_OK);

	result = testcase.pfMap(bs);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == testcase.cbInput);
	fail_unless(
		memcmp(bs_get_buffer(bs), testcase.rgbOutput, testcase.cbInput) == 0
	);

	bs_free(bs);
}
END_TEST

START_TEST(test_maps_empty_bs)
{
	BSresult (*pfMap)(BS *) = rgfMaps[_i];
	BS *bs = bs_create();
	BSresult result;

	result = pfMap(bs);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 0);

	bs_free(bs);
}
END_TEST

START_TEST(test_maps_null_bs)
{
	BSresult (*pfMap)(BS *) = rgfMaps[_i];
	BSresult result;

	result = pfMap(NULL);
	fail_unless(result == BS_NULL);
}
END_TEST


/* ==================== */
/* NULL parameter tests */
/* ==================== */

START_TEST(test_generic_map_null_bs)
{
	BSresult result;

	result = bs_map(NULL, (BSbyte (*)(BSbyte)) 0xDEADBEEF);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_generic_map_null_operation)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_map(bs, NULL);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST


int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Mapping");
	TCase *tc_core = tcase_create("Core");
	size_t cTestcases = sizeof(rgTestcases) / sizeof(struct BSMapTestcase);
	SRunner *sr;
	int number_failed;

	tcase_add_loop_test(tc_core, test_maps,          0, cTestcases);
	tcase_add_loop_test(tc_core, test_maps_empty_bs, 0, C_MAPS);
	tcase_add_loop_test(tc_core, test_maps_null_bs,  0, C_MAPS);

	tcase_add_test(tc_core, test_generic_map_null_bs);
	tcase_add_test(tc_core, test_generic_map_null_operation);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
