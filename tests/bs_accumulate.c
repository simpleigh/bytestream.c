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

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

static unsigned int
byte_counts_target[3] = { 1, 4, 5 };

static BSresult
accumulate_operation(BSbyte byte, void *data)
{
	unsigned int *pbCounts = (unsigned int *) data;
	pbCounts[byte]++;
	return BS_OK;
}

START_TEST(test_accumulate)
{
	BS *bs = bs_create();
	BSresult result;
	unsigned int byte_counts[3] = { 0, 0, 0 };

	result = bs_load(bs, "\x0\x1\x2\x1\x1\x1\x2\x2\x2\x2", 10);
	fail_unless(result == BS_OK);

	result = bs_accumulate(bs, accumulate_operation, &byte_counts);
	fail_unless(result == BS_OK);
	fail_unless(memcmp(byte_counts, byte_counts_target, 3 * sizeof(int)) == 0);

	bs_free(bs);
}
END_TEST

static BSresult
accumulate_bad_operation(BSbyte byte, void *data)
{
	UNUSED(byte);
	UNUSED(data);
	return 999;
}

START_TEST(test_accumulate_invalid)
{
	BS *bs = bs_create_size(1);
	BSresult result;

	result = bs_accumulate(bs, accumulate_bad_operation, &result);
	fail_unless(result == 999);

	bs_free(bs);
}
END_TEST

struct accumulate_test_case {
	BSbyte byte;
	unsigned int sum;
	unsigned int bits;
};

static struct accumulate_test_case
accumulate_tests[16] = {
	{ '\x00',   0, 0 },
	{ '\x01',   1, 1 },
	{ '\x02',   2, 1 },
	{ '\x04',   4, 1 },
	{ '\x08',   8, 1 },
	{ '\x10',  16, 1 },
	{ '\x20',  32, 1 },
	{ '\x40',  64, 1 },
	{ '\x80', 128, 1 },
	{ '\x03',   3, 2 },
	{ '\x07',   7, 3 },
	{ '\x0F',  15, 4 },
	{ '\x1F',  31, 5 },
	{ '\x3F',  63, 6 },
	{ '\x7F', 127, 7 },
	{ '\xFF', 255, 8 },
};

START_TEST(test_sum_starts_zero)
{
	BS *bs = bs_create();
	unsigned int sum = 999;
	BSresult result;

	result = bs_accumulate_sum(bs, &sum);
	fail_unless(result == BS_OK);
	fail_unless(sum == 0);

	bs_free(bs);
}
END_TEST

START_TEST(test_sum)
{
	BS *bs = bs_create_size(1);
	unsigned int sum;
	BSresult result;

	bs_set_byte(bs, 0, accumulate_tests[_i].byte);

	result = bs_accumulate_sum(bs, &sum);
	fail_unless(result == BS_OK);
	fail_unless(sum == accumulate_tests[_i].sum);

	bs_free(bs);
}
END_TEST

START_TEST(test_sum_long)
{
	BS *bs = bs_create();
	unsigned int sum;
	BSresult result;

	bs_load(bs, (BSbyte *) "Test input", 10);

	result = bs_accumulate_sum(bs, &sum);
	fail_unless(result == BS_OK);
	fail_unless(sum == 1008);

	bs_free(bs);
}
END_TEST

START_TEST(test_bits_starts_zero)
{
	BS *bs = bs_create();
	unsigned int bits = 999;
	BSresult result;

	result = bs_accumulate_bits(bs, &bits);
	fail_unless(result == BS_OK);
	fail_unless(bits == 0);

	bs_free(bs);
}
END_TEST

START_TEST(test_bits)
{
	BS *bs = bs_create_size(1);
	unsigned int bits;
	BSresult result;

	bs_set_byte(bs, 0, accumulate_tests[_i].byte);

	result = bs_accumulate_bits(bs, &bits);
	fail_unless(result == BS_OK);
	fail_unless(bits == accumulate_tests[_i].bits);

	bs_free(bs);
}
END_TEST

START_TEST(test_bits_long)
{
	BS *bs = bs_create();
	unsigned int bits;
	BSresult result;

	bs_load(bs, (BSbyte *) "Test input", 10);

	result = bs_accumulate_bits(bs, &bits);
	fail_unless(result == BS_OK);
	fail_unless(bits == 38);

	bs_free(bs);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Accumulators");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_test(tc_core, test_accumulate);
	tcase_add_test(tc_core, test_accumulate_invalid);
	tcase_add_test(tc_core, test_sum_starts_zero);
	tcase_add_loop_test(tc_core, test_sum, 0, 16);
	tcase_add_test(tc_core, test_sum_long);
	tcase_add_test(tc_core, test_bits_starts_zero);
	tcase_add_loop_test(tc_core, test_bits, 0, 16);
	tcase_add_test(tc_core, test_bits_long);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
