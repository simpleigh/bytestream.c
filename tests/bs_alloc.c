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
#include "../src/bs_alloc.h"
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

	for (ibIndex = 0; ibIndex < cbSize; ibIndex++) {
		bs_set_byte(bs, ibIndex, 0);
	}

	bs_free(bs);
}
END_TEST

START_TEST(test_allocate_output)
{
	size_t cbSize = test_create_sizes[_i], cbOutput, ibIndex;
	BSbyte *pbOutput;
	BSresult result;

	result = bs_malloc_output(cbSize, &pbOutput, &cbOutput);

	fail_unless(result == BS_OK);
	fail_unless(cbOutput == cbSize);
	if (cbSize == 0) {
		fail_unless(pbOutput == NULL);
	} else {
		fail_unless(pbOutput != NULL);
	}

	/* Confirm the allocation worked by trying to write to each byte */
	for (ibIndex = 0; ibIndex < cbSize; ibIndex++) {
		pbOutput[ibIndex] = ibIndex;
	}

	free(pbOutput);
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
	tcase_add_loop_test(tc_core, test_allocate_output, 0, 3);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
