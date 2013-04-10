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

struct base64_testcase_struct {
	BSbyte rgBytes[3];
	size_t cbBytes;
	char base64[5];
};

static const struct base64_testcase_struct
base64_testcases[11] = {
	{ {   0,   0,   0 }, 1, "AA==" },
	{ {   0,   0,   0 }, 2, "AAA=" },
	{ {   0,   0,   0 }, 3, "AAAA" },
	{ { 255, 255, 255 }, 1, "/w==" },
	{ { 255, 255, 255 }, 2, "//8=" },
	{ { 255, 255, 255 }, 3, "////" },
	{ {   0,   0,  46 }, 3, "AAAu" },
	{ { 'M', 'a', 'n' }, 3, "TWFu" },
	{ { 'a', 'n', 'y' }, 3, "YW55" },
	{ { 151,  79, 191 }, 3, "l0+/" },
	{ {   0,  16, 131 }, 3, "ABCD" },
};

START_TEST(test_load)
{
	BS *bs = bs_create();
	BSresult result;
	size_t ibByte;

	result = bs_load_base64(bs, &(base64_testcases[_i].base64), 4);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == base64_testcases[_i].cbBytes);

	for (ibByte = 0; ibByte < base64_testcases[_i].cbBytes; ibByte++) {
		fail_unless(
			bs_byte_get(bs, ibByte) == base64_testcases[_i].rgBytes[ibByte]
		);
	}

	bs_free(bs);
}
END_TEST

START_TEST(test_load_many)
{
	BS *bs = bs_create();
	BSresult result;
	BSbyte expected[] = "easure.";
	size_t ibByte;

	result = bs_load_base64(bs, "ZWFzdXJlLg==", 12);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 7);

	for (ibByte = 0; ibByte < 7; ibByte++) {
		fail_unless(bs_byte_get(bs, ibByte) == expected[ibByte]);
	}

	bs_free(bs);
}
END_TEST

static const char
bad_chars[6] = { '!', '"', '#', '\0', 127, 255 };

START_TEST(test_bad_chars)
{
	BS *bs = bs_create();
	char base64[] = "A*==";
	BSresult result;

	base64[1] = bad_chars[_i];
	result = bs_load_base64(bs, base64, 4);

	fail_unless(result == BS_INVALID);
	bs_free(bs);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Base64");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_loop_test(tc_core, test_load, 0, 11);
	tcase_add_test(tc_core, test_load_many);
	tcase_add_loop_test(tc_core, test_bad_chars, 0, 6);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
