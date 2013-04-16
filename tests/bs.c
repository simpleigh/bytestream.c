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
#include <signal.h>
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

START_TEST(test_get_range)
{
	BS *bs = bs_create_size(5);
	bs_get_byte(bs, 5);
}
END_TEST

START_TEST(test_set_range)
{
	BS *bs = bs_create_size(5);
	bs_get_byte(bs, 5);
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
	tcase_add_test_raise_signal(tc_core, test_get_range, SIGABRT);
	tcase_add_test_raise_signal(tc_core, test_set_range, SIGABRT);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
