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
#include "../lib/bs_internal.h"
#include <check.h>
#include <stdlib.h>

START_TEST(test_create)
{
	BS *bs = bs_create();

	fail_unless(bs != NULL);
	fail_unless(bs_size(bs) == 0);
	fail_unless(bs->cbBytes == 0);
	fail_unless(bs->pbBytes == NULL);
	fail_unless(bs->cbBuffer == 0);
	fail_unless(bs->cbStream == 0);

	bs_free(bs);
}
END_TEST

static size_t test_create_sizes[3] = { 0, 1, 5 };

START_TEST(test_create_size)
{
	size_t ibIndex, cbSize = test_create_sizes[_i];
	BS *bs = bs_create_size(cbSize);

	fail_unless(bs != NULL);
	fail_unless(bs_size(bs) == cbSize);
	fail_unless(bs->cbBytes == cbSize);
	if (cbSize == 0) {
		fail_unless(bs->pbBytes == NULL);
	} else {
		fail_unless(bs->pbBytes != NULL);
	}
	fail_unless(bs->cbBuffer == cbSize);
	fail_unless(bs->cbStream == 0);

	/* Confirm the allocation worked by trying to write to each byte */
	for (ibIndex = 0; ibIndex < cbSize; ibIndex++) {
		bs_set_byte(bs, ibIndex, 0);
	}

	bs_free(bs);
}
END_TEST

static size_t test_change_sizes[2] = { 1, 8 };

START_TEST(test_change_size)
{
	size_t ibIndex, cbSize = test_change_sizes[_i];
	BS *bs = bs_create_size(5);
	BSresult result;
	BSbyte *pbBytes;

	pbBytes = bs->pbBytes;

	result = bs_malloc(bs, cbSize);

	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == cbSize);
	fail_unless(bs->cbBytes == cbSize);
	fail_unless(bs->pbBytes == pbBytes);
	fail_unless(bs->cbBuffer == (cbSize > 5) ? cbSize : 5);
	fail_unless(bs->cbStream == 0);

	for (ibIndex = 0; ibIndex < cbSize; ibIndex++) {
		bs_set_byte(bs, ibIndex, 0);
	}

	bs_free(bs);
}
END_TEST

START_TEST(test_get_buffer_on_empty_stream)
{
	BS *bs = bs_create();
	BSbyte *buffer;

	buffer = bs_get_buffer(bs);
	fail_unless(buffer == NULL);

	bs_free(bs);
}
END_TEST

START_TEST(test_get_buffer_on_full_stream)
{
	BS *bs = bs_create_size(10);
	BSbyte *buffer;

	buffer = bs_get_buffer(bs);
	fail_unless(buffer != NULL);
	fail_unless(buffer == bs->pbBytes);

	bs_free(bs);
}
END_TEST

START_TEST(test_set_buffer_on_empty_stream)
{
	BS *bs = bs_create();
	BSbyte *buffer = (BSbyte *) 0xdeadbeef;
	BSresult result;

	result = bs_set_buffer(bs, buffer, 5);
	fail_unless(result == BS_OK);
	fail_unless(bs->cbBytes == 5);
	fail_unless(bs->pbBytes == buffer);
	fail_unless(bs->cbBuffer == 5);
	fail_unless(bs->cbStream == 0);

	bs_unset_buffer(bs);
	bs_free(bs);
}
END_TEST

START_TEST(test_set_buffer_on_full_stream)
{
	BS *bs = bs_create_size(10);
	BSbyte *buffer = (BSbyte *) 0xdeadbeef;
	BSresult result;

	result = bs_set_buffer(bs, buffer, 5);
	fail_unless(result == BS_OK);
	fail_unless(bs->cbBytes == 5);
	fail_unless(bs->pbBytes == buffer);
	fail_unless(bs->cbBuffer == 5);
	fail_unless(bs->cbStream == 0);

	bs_unset_buffer(bs);
	bs_free(bs);
}
END_TEST

START_TEST(test_set_buffer_on_null_bytestream)
{
	BSbyte *buffer = (BSbyte *) 0xdeadbeef;
	BSresult result;

	result = bs_set_buffer(NULL, buffer, 5);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_set_buffer_null)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_set_buffer(bs, NULL, 5);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST

START_TEST(test_set_buffer_zero_length)
{
	BS *bs = bs_create();
	BSbyte *buffer = (BSbyte *) 0xdeadbeef;
	BSresult result;

	result = bs_set_buffer(bs, buffer, 0);
	fail_unless(result == BS_INVALID);

	bs_free(bs);
}
END_TEST

START_TEST(test_set_get_buffer)
{
	BS *bs = bs_create();
	BSbyte *buffer = (BSbyte *) 0xdeadbeef;
	BSresult result;

	result = bs_set_buffer(bs, buffer, 10);
	fail_unless(result == BS_OK);

	buffer = bs_get_buffer(bs);
	fail_unless(buffer == (BSbyte *) 0xdeadbeef);

	bs_unset_buffer(bs);
	bs_free(bs);
}
END_TEST

START_TEST(test_unset_buffer)
{
	BS *bs = bs_create();

	bs->cbBytes = 10;
	bs->pbBytes = (BSbyte *) 0xdeadbeef;
	bs->cbBuffer = 20;
	bs->cbStream = 5;

	bs_unset_buffer(bs);
	fail_unless(bs->cbBytes == 0);
	fail_unless(bs->pbBytes == NULL);
	fail_unless(bs->cbBuffer == 0);
	fail_unless(bs->cbStream == 0);

	bs_free(bs);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Allocations");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_test(tc_core, test_create);
	tcase_add_loop_test(tc_core, test_create_size, 0, 3);
	tcase_add_loop_test(tc_core, test_change_size, 0, 2);
	tcase_add_test(tc_core, test_get_buffer_on_empty_stream);
	tcase_add_test(tc_core, test_get_buffer_on_full_stream);
	tcase_add_test(tc_core, test_set_buffer_on_empty_stream);
	tcase_add_test(tc_core, test_set_buffer_on_full_stream);
	tcase_add_test(tc_core, test_set_buffer_on_null_bytestream);
	tcase_add_test(tc_core, test_set_buffer_null);
	tcase_add_test(tc_core, test_set_buffer_zero_length);
	tcase_add_test(tc_core, test_set_get_buffer);
	tcase_add_test(tc_core, test_unset_buffer);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
