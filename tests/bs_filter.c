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

static BSbyte
byte_values[10] = { 0, 1, 2, 1, 1, 1, 2, 2, 2, 2 };

static unsigned int
byte_counts[3] = { 0, 0, 0 };

static unsigned int
byte_counts_target[3] = { 1, 4, 5 };

static BSbyte
operation(BSbyte byte)
{
	byte_counts[byte]++;
	return 0;
}

START_TEST(test_filter)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_load(bs, byte_values, 10);
	fail_unless(result == BS_OK);

	result = bs_filter(bs, operation);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 10);
	fail_unless(
		memcmp(byte_counts, byte_counts_target, 3) == 0
	);

	bs_free(bs);
}
END_TEST

struct filter_testcase_struct {
	BSresult (*filter_function) (BS *bs);
	char input[3];
	char output[3];
};

static const struct filter_testcase_struct
filter_testcases[16] = {
	{ bs_filter_uppercase, "00", "00" },
	{ bs_filter_uppercase, "61", "41" },
	{ bs_filter_uppercase, "7a", "5a" },
	{ bs_filter_uppercase, "ff", "ff" },
	{ bs_filter_lowercase, "00", "00" },
	{ bs_filter_lowercase, "41", "61" },
	{ bs_filter_lowercase, "5a", "7a" },
	{ bs_filter_lowercase, "ff", "ff" },
	{ bs_filter_not,       "01", "fe" },
	{ bs_filter_not,       "23", "dc" },
	{ bs_filter_not,       "45", "ba" },
	{ bs_filter_not,       "67", "98" },
	{ bs_filter_not,       "89", "76" },
	{ bs_filter_not,       "ab", "54" },
	{ bs_filter_not,       "cd", "32" },
	{ bs_filter_not,       "ef", "10" },
};

START_TEST(test_filter_functions)
{
	BS *bs = bs_create();
	BSresult result;
	char *hex;
	size_t length;

	result = bs_load_hex(bs, filter_testcases[_i].input, 2);
	fail_unless(result == BS_OK);

	result = filter_testcases[_i].filter_function(bs);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 1);

	result = bs_save_hex(bs, &hex, &length);
	fail_unless(result == BS_OK);
	fail_unless(hex != NULL);
	fail_unless(length == 2);
	fail_unless(strcmp(hex, filter_testcases[_i].output) == 0);

	free(hex);
	bs_free(bs);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Filters");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_test(tc_core, test_filter);
	tcase_add_loop_test(tc_core, test_filter_functions, 0, 16);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
