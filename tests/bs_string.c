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

static unsigned char test_binary[5] = { '\x0', '\x1', '\x7f', '\x80', '\xff' };

START_TEST(test_load_binary)
{
	BS *bs = bs_create();
	BSresult result;
	size_t ibIndex;

	result = bs_load_binary(bs, test_binary, 5);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 5);

	for (ibIndex = 0; ibIndex < bs_size(bs); ibIndex++) {
		fail_unless(bs_byte_get(bs, ibIndex) == test_binary[ibIndex]);
	}

	bs_free(bs);
}
END_TEST

static char test_string[] = "\x0\x1\x7f";

START_TEST(test_load_string)
{
	BS *bs = bs_create();
	BSresult result;
	size_t ibIndex;

	result = bs_load_string(bs, test_string, 3);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 3);

	for (ibIndex = 0; ibIndex < bs_size(bs); ibIndex++) {
		fail_unless(bs_byte_get(bs, ibIndex) == (BSbyte)test_string[ibIndex]);
	}

	bs_free(bs);
}
END_TEST

static char test_bad_string[] = { -1, '\0' };

START_TEST(test_load_bad_string)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_load_string(bs, test_bad_string, 1);
	fail_unless(result == BS_INVALID);
	fail_unless(bs_size(bs) == 0);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Strings");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_test(tc_core, test_load_binary);
	tcase_add_test(tc_core, test_load_string);
	tcase_add_test(tc_core, test_load_bad_string);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
