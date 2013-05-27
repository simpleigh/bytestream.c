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

static BSfilter
include_all(BSbyte byte)
{
	UNUSED(byte);
	return BS_INCLUDE;
}

static BSfilter
exclude_all(BSbyte byte)
{
	UNUSED(byte);
	return BS_EXCLUDE;
}

static BSresult
filter_include_all(BS *bs)
{
	return bs_filter(bs, include_all);
}

static BSresult
filter_exclude_all(BS *bs)
{
	return bs_filter(bs, exclude_all);
}


/* ========= */
/* Testcases */
/* ========= */

#define C_FILTERS 3

static BSresult (*rgfFilters[C_FILTERS])(BS *) = {
	filter_include_all,
	filter_exclude_all,
	bs_filter_whitespace,
};

struct BSFilterTestcase {
	BSresult (*pfFilter)(BS *); /* Function to test */
	BSbyte *rgbInput;           /* Starting bytestream contents */
	size_t cbInput;             /* Starting bytestream length */
	const BSbyte *rgbOutput;    /* Expected bytestream contents */
	size_t cbOutput;            /* Expected bytestream length */
};

static struct BSFilterTestcase
rgTestcases[] = {
	{ filter_include_all,   "12345",      5, "12345",    5 },
	{ filter_exclude_all,   "12345",      5, "",         0 },
	{ bs_filter_whitespace, "\x9\xA\xD ", 4, "",         0 },
	{ bs_filter_whitespace, "aAzZ09+.",   8, "aAzZ09+.", 8 },
	{ bs_filter_whitespace, "test str",   8, "teststr",  7 },
};


/* ============== */
/* Testcase tests */
/* ============== */

START_TEST(test_filters)
{
	struct BSFilterTestcase testcase = rgTestcases[_i];
	BS *bs = bs_create();
	BSresult result;

	result = bs_load(bs, testcase.rgbInput, testcase.cbInput);
	fail_unless(result == BS_OK);

	result = testcase.pfFilter(bs);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == testcase.cbOutput);
	fail_unless(
		memcmp(bs_get_buffer(bs), testcase.rgbOutput, testcase.cbOutput) == 0
	);

	bs_free(bs);
}
END_TEST

START_TEST(test_filters_empty_bs)
{
	BSresult (*pfFilter)(BS *) = rgfFilters[_i];
	BS *bs = bs_create();
	BSresult result;

	result = pfFilter(bs);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 0);

	bs_free(bs);
}
END_TEST

START_TEST(test_filters_null_bs)
{
	BSresult (*pfFilter)(BS *) = rgfFilters[_i];
	BSresult result;

	result = pfFilter(NULL);
	fail_unless(result == BS_NULL);
}
END_TEST


/* ==================== */
/* NULL parameter tests */
/* ==================== */

START_TEST(test_generic_filter_null_bs)
{
	BSresult result;

	result = bs_filter(NULL, (BSfilter (*)(BSbyte)) 0xDEADBEEF);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_generic_filter_null_operation)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_filter(bs, NULL);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST


int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Filters");
	TCase *tc_core = tcase_create("Core");
	size_t cTestcases = sizeof(rgTestcases) / sizeof(struct BSFilterTestcase);
	SRunner *sr;
	int number_failed;

	tcase_add_loop_test(tc_core, test_filters,          0, cTestcases);
	tcase_add_loop_test(tc_core, test_filters_empty_bs, 0, C_FILTERS);
	tcase_add_loop_test(tc_core, test_filters_null_bs,  0, C_FILTERS);

	tcase_add_test(tc_core, test_generic_filter_null_bs);
	tcase_add_test(tc_core, test_generic_filter_null_operation);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_set_fork_status(sr, CK_NOFORK);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
