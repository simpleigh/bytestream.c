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

/* =============================== */
/* Tiny functions used for testing */
/* =============================== */

static BSresult
fold_bad_operation(BSbyte byte, void *data)
{
	UNUSED(byte);
	UNUSED(data);
	return 999;
}


/* ========= */
/* Testcases */
/* ========= */

struct BSFoldType {
	BSresult (*pfFold)(const BS *, unsigned int *); /* Function to test */
	unsigned int initial;                           /* Initial value */
};

static struct BSFoldType
rgFoldTypes[] = {
	{ bs_fold_sum,      0 },
	{ bs_fold_bitcount, 0 },
};

struct BSFoldTestcase {
	BSresult (*pfFold)(const BS *, unsigned int *); /* Function to test */
	BSbyte *rgbInput;                               /* Starting bytestream contents */
	size_t cbInput;                                 /* Starting bytestream length */
	unsigned int output;                            /* Expected output */
};


static struct BSFoldTestcase
rgTestcases[] = {
	{ bs_fold_sum,      "\0\1",        2,    1 },
	{ bs_fold_sum,      "\1\0",        2,    1 },
	{ bs_fold_sum,      "\x00",        1,    0 },
	{ bs_fold_sum,      "\x01",        1,    1 },
	{ bs_fold_sum,      "\x02",        1,    2 },
	{ bs_fold_sum,      "\x04",        1,    4 },
	{ bs_fold_sum,      "\x08",        1,    8 },
	{ bs_fold_sum,      "\x10",        1,   16 },
	{ bs_fold_sum,      "\x20",        1,   32 },
	{ bs_fold_sum,      "\x40",        1,   64 },
	{ bs_fold_sum,      "\x80",        1,  128 },
	{ bs_fold_sum,      "\x03",        1,    3 },
	{ bs_fold_sum,      "\x07",        1,    7 },
	{ bs_fold_sum,      "\x0F",        1,   15 },
	{ bs_fold_sum,      "\x1F",        1,   31 },
	{ bs_fold_sum,      "\x3F",        1,   63 },
	{ bs_fold_sum,      "\x7F",        1,  127 },
	{ bs_fold_sum,      "\xFF",        1,  255 },
	{ bs_fold_sum,      "Test input", 10, 1008 },
	{ bs_fold_bitcount, "\0\1",        2,    1 },
	{ bs_fold_bitcount, "\1\0",        2,    1 },
	{ bs_fold_bitcount, "\x00",        1,    0 },
	{ bs_fold_bitcount, "\x01",        1,    1 },
	{ bs_fold_bitcount, "\x02",        1,    1 },
	{ bs_fold_bitcount, "\x04",        1,    1 },
	{ bs_fold_bitcount, "\x08",        1,    1 },
	{ bs_fold_bitcount, "\x10",        1,    1 },
	{ bs_fold_bitcount, "\x20",        1,    1 },
	{ bs_fold_bitcount, "\x40",        1,    1 },
	{ bs_fold_bitcount, "\x80",        1,    1 },
	{ bs_fold_bitcount, "\x03",        1,    2 },
	{ bs_fold_bitcount, "\x07",        1,    3 },
	{ bs_fold_bitcount, "\x0F",        1,    4 },
	{ bs_fold_bitcount, "\x1F",        1,    5 },
	{ bs_fold_bitcount, "\x3F",        1,    6 },
	{ bs_fold_bitcount, "\x7F",        1,    7 },
	{ bs_fold_bitcount, "\xFF",        1,    8 },
	{ bs_fold_bitcount, "Test input", 10,   38 },
};


/* ============== */
/* Testcase tests */
/* ============== */

START_TEST(test_folds)
{
	struct BSFoldTestcase testcase = rgTestcases[_i];
	BS *bs = bs_create();
	BSresult result;
	unsigned int output;

	result = bs_load(bs, testcase.rgbInput, testcase.cbInput);
	fail_unless(result == BS_OK);

	result = testcase.pfFold(bs, &output);
	fail_unless(result == BS_OK);
	fail_unless(output == testcase.output);
	fail_unless(
		memcmp(bs_get_buffer(bs), testcase.rgbInput, testcase.cbInput) == 0
	);

	bs_free(bs);
}
END_TEST

START_TEST(test_folds_empty_bs)
{
	struct BSFoldType fold = rgFoldTypes[_i];
	BS *bs = bs_create();
	BSresult result;
	unsigned int output;

	result = fold.pfFold(bs, &output);
	fail_unless(result == BS_OK);
	fail_unless(output == fold.initial);

	bs_free(bs);
}
END_TEST

START_TEST(test_folds_null_bs)
{
	struct BSFoldType fold = rgFoldTypes[_i];
	BSresult result;

	result = fold.pfFold(NULL, (unsigned int *) 0xDEADBEEF);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_folds_null_output)
{
	struct BSFoldType fold = rgFoldTypes[_i];
	BS *bs = bs_create();
	BSresult result;

	result = fold.pfFold(bs, NULL);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST


/* =========== */
/* Other tests */
/* =========== */

START_TEST(test_fold_bad_operation)
{
	BS *bs = bs_create_size(1);
	BSresult result;

	result = bs_fold(bs, fold_bad_operation, (void *) 0xDEADBEEF);
	fail_unless(result == 999);

	bs_free(bs);
}
END_TEST

START_TEST(test_fold_bad_operation_empty_bs)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_fold(bs, fold_bad_operation, (void *) 0xDEADBEEF);
	fail_unless(result == BS_OK);

	bs_free(bs);
}
END_TEST


/* ==================== */
/* NULL parameter tests */
/* ==================== */

START_TEST(test_generic_fold_null_bs)
{
	BSresult result;

	result = bs_fold(
		NULL,
		(BSresult (*)(BSbyte, void *)) 0xDEADBEEF,
		(void *) 0xDEADBEEF
	);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_generic_fold_null_operation)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_fold(bs, NULL, (void *) 0xDEADBEEF);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST


int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Folding");
	TCase *tc_core = tcase_create("Core");
	size_t cTestcases = sizeof(rgTestcases) / sizeof(struct BSFoldTestcase);
	size_t cFoldTypes = sizeof(rgFoldTypes) / sizeof(struct BSFoldType);
	SRunner *sr;
	int number_failed;

	tcase_add_loop_test(tc_core, test_folds,             0, cTestcases);
	tcase_add_loop_test(tc_core, test_folds_empty_bs,    0, cFoldTypes);
	tcase_add_loop_test(tc_core, test_folds_null_bs,     0, cFoldTypes);
	tcase_add_loop_test(tc_core, test_folds_null_output, 0, cFoldTypes);

	tcase_add_test(tc_core, test_fold_bad_operation);
	tcase_add_test(tc_core, test_fold_bad_operation_empty_bs);

	tcase_add_test(tc_core, test_generic_fold_null_bs);
	tcase_add_test(tc_core, test_generic_fold_null_operation);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
