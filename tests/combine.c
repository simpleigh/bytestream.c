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

#include "libbs.h"
#include <check.h>
#include <stdlib.h>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif


/* =============================== */
/* Tiny functions used for testing */
/* =============================== */

static BSbyte
overwrite_byte(BSbyte byte1, BSbyte byte2)
{
	UNUSED(byte1);
	return byte2;
}

static BSresult
combine_overwrite(BS *bs, const BS *operand)
{
	return bs_combine(bs, operand, overwrite_byte);
}


/* ========= */
/* Testcases */
/* ========= */

#define C_COMBINATIONS 6

static BSresult (*rgfCombinations[C_COMBINATIONS])(BS *, const BS *) = {
	combine_overwrite,
	bs_combine_xor,
	bs_combine_or,
	bs_combine_and,
	bs_combine_add,
	bs_combine_sub,
};

struct BSCombineTestcase {
	BSresult (*pfCombine)(BS *, const BS *); /* Function to test */
	const BSbyte *rgbInput;                  /* Starting bytestream contents */
	size_t cbInput;                          /* Starting bytestream length */
	const BSbyte *rgbOperand;                /* Starting operand contents */
	size_t cbOperand;                        /* Starting operand length */
	const BSbyte *rgbOutput;                 /* Expected bytestream contents */
};

static struct BSCombineTestcase
rgTestcases[] = {
	{ combine_overwrite, "        ",                         8, "1",                1, "11111111"                         },
	{ combine_overwrite, "        ",                         8, "1 ",               2, "1 1 1 1 "                         },
	{ combine_overwrite, "        ",                         8, "1  ",              3, "1  1  1 "                         },
	{ combine_overwrite, "        ",                         8, "1   ",             4, "1   1   "                         },
	{ combine_overwrite, "    ",                             4, "12345678",         8, "1234"                             },
	{ bs_combine_xor,    "\x00\x00\x01\x01",                 4, "\x00\x01\x00\x01", 4, "\x00\x01\x01\x00"                 },
	{ bs_combine_xor,    "\x01\x23\x45\x67\x89\xAB\xCD\xEF", 8, "\xAA\xBB\xCC",     3, "\xAB\x98\x89\xCD\x32\x67\x67\x54" },
	{ bs_combine_or,     "\x00\x00\x01\x01",                 4, "\x00\x01\x00\x01", 4, "\x00\x01\x01\x01"                 },
	{ bs_combine_or,     "\x01\x23\x45\x67\x89\xAB\xCD\xEF", 8, "\xAA\xBB\xCC",     3, "\xAB\xBB\xCD\xEF\xBB\xEF\xEF\xFF" },
	{ bs_combine_and,    "\x00\x00\x01\x01",                 4, "\x00\x01\x00\x01", 4, "\x00\x00\x00\x01"                 },
	{ bs_combine_and,    "\x01\x23\x45\x67\x89\xAB\xCD\xEF", 8, "\xAA\xBB\xCC",     3, "\x00\x23\x44\x22\x89\x88\x88\xAB" },
	{ bs_combine_add,    "1234",                             4, "\x01",             1, "2345"                             },
	{ bs_combine_add,    "\x01\x23\x45\x67\x89\xAB\xCD\xEF", 8, "\xAA\xBB\xCC",     3, "\xAB\xDE\x11\x11\x44\x77\x77\xAA" },
	{ bs_combine_sub,    "2345",                             4, "\x01",             1, "1234"                             },
	{ bs_combine_sub,    "\x01\x23\x45\x67\x89\xAB\xCD\xEF", 8, "\xAA\xBB\xCC",     3, "\x57\x68\x79\xBD\xCE\xDF\x23\x34" },
};


/* ============== */
/* Testcase tests */
/* ============== */

START_TEST(test_combinations)
{
	struct BSCombineTestcase testcase = rgTestcases[_i];
	BS *bs      = bs_create();
	BS *operand = bs_create();
	BSresult result;

	result = bs_load(bs, testcase.rgbInput, testcase.cbInput);
	fail_unless(result == BS_OK);

	result = bs_load(operand, testcase.rgbOperand, testcase.cbOperand);
	fail_unless(result == BS_OK);

	result = testcase.pfCombine(bs, operand);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == testcase.cbInput);
	fail_unless(
		memcmp(bs_get_buffer(bs), testcase.rgbOutput, testcase.cbInput) == 0
	);
	fail_unless(
		memcmp(
			bs_get_buffer(operand),
			testcase.rgbOperand,
			testcase.cbOperand
		) == 0
	);

	bs_free(bs);
	bs_free(operand);
}
END_TEST

START_TEST(test_combinations_empty_bs)
{
	BSresult (*pfCombine)(BS *, const BS *) = rgfCombinations[_i];
	BS *bs = bs_create(), *operand = bs_create_size(5);
	BSresult result;

	result = pfCombine(bs, operand);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 0);

	bs_free(bs);
	bs_free(operand);
}
END_TEST

START_TEST(test_combinations_empty_operand)
{
	BSresult (*pfCombine)(BS *, const BS *) = rgfCombinations[_i];
	BS *bs = bs_create(), *operand = bs_create();
	BSresult result;

	result = bs_load(bs, (BSbyte *) "test", 4);
	fail_unless(result == BS_OK);

	result = pfCombine(bs, operand);
	fail_unless(result == BS_INVALID);
	fail_unless(bs_size(bs) == 4);
	fail_unless(memcmp(bs_get_buffer(bs), "test", 4) == 0);

	bs_free(bs);
	bs_free(operand);
}
END_TEST

START_TEST(test_combinations_empty_parameters)
{
	BSresult (*pfCombine)(BS *, const BS *) = rgfCombinations[_i];
	BS *bs = bs_create(), *operand = bs_create();
	BSresult result;

	result = pfCombine(bs, operand);
	fail_unless(result == BS_INVALID);
	fail_unless(bs_size(bs) == 0);

	bs_free(bs);
	bs_free(operand);
}
END_TEST

START_TEST(test_combinations_null_bs)
{
	BSresult (*pfCombine)(BS *, const BS *) = rgfCombinations[_i];
	BS *operand = bs_create();
	BSresult result;

	result = pfCombine(NULL, operand);
	fail_unless(result == BS_NULL);

	bs_free(operand);
}
END_TEST

START_TEST(test_combinations_null_operand)
{
	BSresult (*pfCombine)(BS *, const BS *) = rgfCombinations[_i];
	BS *bs = bs_create();
	BSresult result;

	result = pfCombine(bs, NULL);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST


/* ==================== */
/* NULL parameter tests */
/* ==================== */

START_TEST(test_generic_combination_null_bs)
{
	BS *operand = bs_create();
	BSresult result;

	result = bs_combine(NULL, operand, (BSbyte (*)(BSbyte, BSbyte)) 0xDEADBEEF);
	fail_unless(result == BS_NULL);

	bs_free(operand);
}
END_TEST

START_TEST(test_generic_combination_null_operand)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_combine(bs, NULL, (BSbyte (*)(BSbyte, BSbyte)) 0xDEADBEEF);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST

START_TEST(test_generic_combination_null_operation)
{
	BS *bs = bs_create(), *operand = bs_create();
	BSresult result;

	result = bs_combine(bs, operand, NULL);
	fail_unless(result == BS_NULL);

	bs_free(bs);
	bs_free(operand);
}
END_TEST


int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Combinations");
	TCase *tc_core = tcase_create("Core");
	size_t cTestcases = sizeof(rgTestcases) / sizeof(struct BSCombineTestcase);
	SRunner *sr;
	int number_failed;

	tcase_add_loop_test(tc_core, test_combinations,                  0, cTestcases);
	tcase_add_loop_test(tc_core, test_combinations_empty_bs,         0, C_COMBINATIONS);
	tcase_add_loop_test(tc_core, test_combinations_empty_operand,    0, C_COMBINATIONS);
	tcase_add_loop_test(tc_core, test_combinations_empty_parameters, 0, C_COMBINATIONS);
	tcase_add_loop_test(tc_core, test_combinations_null_bs,          0, C_COMBINATIONS);
	tcase_add_loop_test(tc_core, test_combinations_null_operand,     0, C_COMBINATIONS);

	tcase_add_test(tc_core, test_generic_combination_null_bs);
	tcase_add_test(tc_core, test_generic_combination_null_operand);
	tcase_add_test(tc_core, test_generic_combination_null_operation);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
