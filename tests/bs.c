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

static size_t test_zero_sizes[] = { 0, 1, 5 };

START_TEST(test_zero)
{
	size_t ibIndex, cbSize = test_zero_sizes[_i];
	BS *bs = bs_create_size(cbSize);

	for (ibIndex = 0; ibIndex < cbSize; ibIndex++) {
		bs_set_byte(bs, ibIndex, 1);
	}

	bs_zero(bs);

	for (ibIndex = 0; ibIndex < cbSize; ibIndex++) {
		fail_unless(bs_get_byte(bs, ibIndex) == 0);
	}

	bs_free(bs);
}
END_TEST

static size_t test_get_set_sizes[] = { 1, 5 };

START_TEST(test_get_set)
{
	size_t ibIndex, cbSize = test_get_set_sizes[_i];
	BS *bs = bs_create_size(cbSize);
	BSbyte bByte = (BSbyte)cbSize;

	bs_zero(bs);
	for (ibIndex = 0; ibIndex < cbSize; ibIndex++) {
		fail_unless(bs_set_byte(bs, ibIndex, bByte) == bByte);
		fail_unless(bs_get_byte(bs, ibIndex) == bByte);
	}

	bs_free(bs);
}
END_TEST

static char
load_data[] = { '\x0', '\x1', '\x7f', '\x80', '\xff' };

static size_t
load_length = 5;

START_TEST(test_load)
{
	BS *bs = bs_create();
	BSresult result;
	size_t ibIndex;

	result = bs_load(bs, (BSbyte *)load_data, load_length);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == load_length);

	for (ibIndex = 0; ibIndex < load_length; ibIndex++) {
		fail_unless(bs_get_byte(bs, ibIndex) == (BSbyte)load_data[ibIndex]);
	}

	bs_free(bs);
}
END_TEST

START_TEST(test_load_empty)
{
	BS *bs = bs_create();
	BSresult result;
	BSbyte *data;

	result = bs_load(bs, data, 0);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 0);

	bs_free(bs);
}
END_TEST

START_TEST(test_load_null_bs)
{
	BSbyte *data = (BSbyte *) 0xDEADBEEF;
	BSresult result;

	result = bs_load(NULL, data, 5);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_load_null_data)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_load(bs, NULL, 5);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST

START_TEST(test_save)
{
	BS *bs = bs_create();
	BSbyte *data;
	size_t length;
	BSresult result;

	bs_load(bs, (BSbyte *)load_data, load_length);

	result = bs_save(bs, &data, &length);
	fail_unless(result == BS_OK);
	fail_unless(data != NULL);
	fail_unless(length == load_length);
	fail_unless(strncmp(data, load_data, load_length) == 0);

	free(data);
	bs_free(bs);
}
END_TEST

START_TEST(test_save_null_bs)
{
	BSbyte *data = (BSbyte *) 0xDEADBEEF;
	size_t length;
	BSresult result;

	result = bs_save(NULL, &data, &length);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_save_null_data)
{
	BS *bs = bs_create();
	size_t length;
	BSresult result;

	result = bs_save(bs, NULL, &length);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST

START_TEST(test_save_null_length)
{
	BS *bs = bs_create();
	BSbyte *data = (BSbyte *) 0xDEADBEEF;
	BSresult result;

	result = bs_save(bs, &data, NULL);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Accessor Functions");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_loop_test(tc_core, test_zero, 0, 3);
	tcase_add_loop_test(tc_core, test_get_set, 0, 2);
	tcase_add_test(tc_core, test_load);
	tcase_add_test(tc_core, test_load_empty);
	tcase_add_test(tc_core, test_load_null_bs);
	tcase_add_test(tc_core, test_load_null_data);
	tcase_add_test(tc_core, test_save);
	tcase_add_test(tc_core, test_save_null_bs);
	tcase_add_test(tc_core, test_save_null_data);
	tcase_add_test(tc_core, test_save_null_length);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
