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

static unsigned int
short_operand_counts[3] = { 0, 0, 0 };

static unsigned int
short_operand_counts_target[3] = { 2, 2, 1 };

static BSbyte
short_operand_operation(BSbyte byte1, BSbyte byte2)
{
	short_operand_counts[byte2]++;
	return 0;
}

START_TEST(test_combine_short_operand)
{
	BS *bs = bs_create_size(5);
	BS *operand = bs_create_size(3);
	size_t ibOperand;
	BSresult result;

	for (ibOperand = 0; ibOperand < 3; ibOperand++) {
		bs_set_byte(operand, ibOperand, (BSbyte) ibOperand);
	}

	result = bs_combine(bs, operand, short_operand_operation);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 5);
	fail_unless(
		memcmp(short_operand_counts, short_operand_counts_target, 3) == 0
	);

	bs_free(bs);
	bs_free(operand);
}
END_TEST

static unsigned int
long_operand_counts[5] = { 0, 0, 0, 0, 0 };

static unsigned int
long_operand_counts_target[5] = { 1, 1, 1, 0, 0};

static BSbyte
long_operand_operation(BSbyte byte1, BSbyte byte2)
{
	long_operand_counts[byte2]++;
	return 0;
}

START_TEST(test_combine_long_operand)
{
	BS *bs = bs_create_size(3);
	BS *operand = bs_create_size(5);
	size_t ibOperand;
	BSresult result;

	for (ibOperand = 0; ibOperand < 3; ibOperand++) {
		bs_set_byte(operand, ibOperand, (BSbyte) ibOperand);
	}

	result = bs_combine(bs, operand, long_operand_operation);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 3);
	fail_unless(
		memcmp(long_operand_counts, long_operand_counts_target, 5) == 0
	);

	bs_free(bs);
	bs_free(operand);
}
END_TEST

START_TEST(test_combine_null_bs1)
{
	BS *bs2 = bs_create();
	BSresult result;

	result = bs_combine(NULL, bs2, short_operand_operation);
	fail_unless(result == BS_NULL);

	bs_free(bs2);
}
END_TEST

START_TEST(test_combine_null_bs2)
{
	BS *bs1 = bs_create();
	BSresult result;

	result = bs_combine(bs1, NULL, short_operand_operation);
	fail_unless(result == BS_NULL);

	bs_free(bs1);
}
END_TEST

struct combine_testcase_struct {
	BSresult (*combine_function) (BS *bs, const BS *operand);
	char output[17];
};

static const struct combine_testcase_struct
combine_testcases[5] = {
	{ bs_combine_xor, "ab9889cd32676754" },
	{ bs_combine_or,  "abbbcdefbbefefff" },
	{ bs_combine_and, "00234422898888ab" },
	{ bs_combine_add, "abde1111447777aa" },
	{ bs_combine_sub, "576879bdcedf2334" },
};

START_TEST(test_combine_functions)
{
	BS *bs = bs_create(), *operand = bs_create();
	BSresult result;
	char *hex;

	result = bs_load_hex(bs, "0123456789abcdef", 16);
	fail_unless(result == BS_OK);

	result = bs_load_hex(operand, "aabbcc", 6);
	fail_unless(result == BS_OK);

	result = combine_testcases[_i].combine_function(bs, operand);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 8);

	hex = malloc(bs_size_hex(bs));
	fail_unless(hex != NULL);

	result = bs_save_hex(bs, hex);
	fail_unless(result == BS_OK);
	fail_unless(hex != NULL);
	fail_unless(strlen(hex) == 16);
	fail_unless(strcmp(hex, combine_testcases[_i].output) == 0);

	free(hex);
	bs_free(bs);
	bs_free(operand);
}
END_TEST

static BSresult (*combine_functions[5]) (BS *bs, const BS *operand) = {
	bs_combine_xor,
	bs_combine_or,
	bs_combine_and,
	bs_combine_add,
	bs_combine_sub
};

START_TEST(test_combine_functions_null_bs1)
{
	BS *bs2 = bs_create();
	BSresult result;

	result = combine_functions[_i](NULL, bs2);
	fail_unless(result == BS_NULL);

	bs_free(bs2);
}
END_TEST

START_TEST(test_combine_functions_null_bs2)
{
	BS *bs1 = bs_create();
	BSresult result;

	result = combine_functions[_i](bs1, NULL);
	fail_unless(result == BS_NULL);

	bs_free(bs1);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Combinations");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_test(tc_core, test_combine_short_operand);
	tcase_add_test(tc_core, test_combine_long_operand);
	tcase_add_test(tc_core, test_combine_null_bs1);
	tcase_add_test(tc_core, test_combine_null_bs2);
	tcase_add_loop_test(tc_core, test_combine_functions, 0, 5);
	tcase_add_loop_test(tc_core, test_combine_functions_null_bs1, 0, 5);
	tcase_add_loop_test(tc_core, test_combine_functions_null_bs2, 0, 5);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
