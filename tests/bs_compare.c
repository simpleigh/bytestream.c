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

struct counts {
	unsigned int bs1[3];
	unsigned int bs2[3];
};

static struct counts
compare_target = { { 1, 2, 2 }, { 3, 2, 0 } };

static BSresult
compare_operation(BSbyte byte1, BSbyte byte2, void *data)
{
	struct counts *counts = (struct counts *) data;
	counts->bs1[byte1]++;
	counts->bs2[byte2]++;
	return BS_OK;
}

START_TEST(test_compare)
{
	BS *bs1 = bs_create_size(5), *bs2 = bs_create_size(5);
	struct counts counts = { { 0, 0, 0 }, { 0, 0, 0 } };
	BSresult result;

	bs_load(bs1, (BSbyte *) "\x0\x1\x1\x2\x2", 5);
	bs_load(bs2, (BSbyte *) "\x0\x0\x0\x1\x1", 5);

	result = bs_compare(bs1, bs2, compare_operation, &counts);
	fail_unless(result == BS_OK);
	fail_unless(memcmp(&counts, &compare_target, sizeof(struct counts)) == 0);

	bs_free(bs1);
	bs_free(bs2);
}
END_TEST

static BSresult
compare_bad_operation(BSbyte byte1, BSbyte byte2, void *data)
{
	UNUSED(byte1);
	UNUSED(byte2);
	UNUSED(data);
	return 999;
}

START_TEST(test_compare_invalid)
{
	BS *bs1 = bs_create_size(1), *bs2 = bs_create_size(1);
	BSresult result;

	result = bs_compare(bs1, bs2, compare_bad_operation, &result);
	fail_unless(result == 999);

	bs_free(bs1);
	bs_free(bs2);
}
END_TEST

START_TEST(test_compare_differing_lengths)
{
	BS *bs1 = bs_create_size(4), *bs2 = bs_create_size(8);
	struct counts counts = { { 0, 0, 0 }, { 0, 0, 0 } };
	BSresult result;

	result = bs_compare(bs1, bs2, compare_operation, &counts);
	fail_unless(result == BS_INVALID);

	bs_free(bs1);
	bs_free(bs2);
}
END_TEST

START_TEST(test_equal_zero_length)
{
	BS *bs1 = bs_create(), *bs2 = bs_create();
	BSresult result;

	result = bs_compare_equal(bs1, bs2);
	fail_unless(result == BS_OK);

	bs_free(bs1);
	bs_free(bs2);
}
END_TEST

START_TEST(test_equal_is_equal)
{
	BS *bs1 = bs_create_size(5), *bs2 = bs_create_size(5);
	BSresult result;

	bs_load(bs1, (BSbyte *) "test input", 10);
	bs_load(bs2, (BSbyte *) "test input", 10);

	result = bs_compare_equal(bs1, bs2);
	fail_unless(result == BS_OK);

	bs_free(bs1);
	bs_free(bs2);
}
END_TEST

START_TEST(test_neq_is_neq)
{
	BS *bs1 = bs_create_size(5), *bs2 = bs_create_size(5);
	BSresult result;

	bs_load(bs1, (BSbyte *) "test input", 10);
	bs_load(bs2, (BSbyte *) "atoei8taos", 10);

	result = bs_compare_equal(bs1, bs2);
	fail_unless(result == BS_INVALID);

	bs_free(bs1);
	bs_free(bs2);
}
END_TEST

struct hamming_test_case {
	BSbyte b1;
	BSbyte b2;
	unsigned int distance;
};

static struct hamming_test_case
hamming_tests[18] = {
	{ '\x00',  '\x0', 0 },
	{ '\x00',  '\x1', 1 },
	{ '\x00',  '\x2', 1 },
	{ '\x00',  '\x4', 1 },
	{ '\x00',  '\x8', 1 },
	{ '\x00', '\x10', 1 },
	{ '\x00', '\x20', 1 },
	{ '\x00', '\x40', 1 },
	{ '\x00', '\x80', 1 },
	{ '\x00', '\xFF', 8 },
	{ '\x01', '\xFF', 7 },
	{ '\x03', '\xFF', 6 },
	{ '\x07', '\xFF', 5 },
	{ '\x0F', '\xFF', 4 },
	{ '\x1F', '\xFF', 3 },
	{ '\x3F', '\xFF', 2 },
	{ '\x7F', '\xFF', 1 },
	{ '\xFF', '\xFF', 0 },
};

START_TEST(test_hamming_byte)
{
	BS *bs1 = bs_create_size(1), *bs2 = bs_create_size(1);
	unsigned int distance;
	BSresult result;

	bs_set_byte(bs1, 0, hamming_tests[_i].b1);
	bs_set_byte(bs2, 0, hamming_tests[_i].b2);

	result = bs_compare_hamming(bs1, bs2, &distance);
	fail_unless(result == BS_OK);
	fail_unless(distance == hamming_tests[_i].distance);

	bs_free(bs1);
	bs_free(bs2);
}
END_TEST

START_TEST(test_hamming_long)
{
	BS *bs1 = bs_create(), *bs2 = bs_create();
	unsigned int distance = 0;
	BSresult result;

	bs_load(bs1, (BSbyte *) "test string one", 14);
	bs_load(bs2, (BSbyte *) "test input two!", 14);

	result = bs_compare_hamming(bs1, bs2, &distance);
	fail_unless(result == BS_OK);
	fail_unless(distance == 23);

	bs_free(bs1);
	bs_free(bs2);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Comparisons");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_test(tc_core, test_compare);
	tcase_add_test(tc_core, test_compare_invalid);
	tcase_add_test(tc_core, test_compare_differing_lengths);
	tcase_add_test(tc_core, test_equal_zero_length);
	tcase_add_test(tc_core, test_equal_is_equal);
	tcase_add_test(tc_core, test_neq_is_neq);
	tcase_add_test(tc_core, test_hamming_long);
	tcase_add_loop_test(tc_core, test_hamming_byte, 0, 18);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
