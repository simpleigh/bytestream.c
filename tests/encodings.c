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

START_TEST(test_decode_bad_encoding)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_decode(bs, "notanencoding", "input", 5);
	fail_unless(result == BS_BAD_ENCODING);

	bs_free(bs);
}
END_TEST

START_TEST(test_encode_size_bad_encoding)
{
	BS *bs = bs_create();
	size_t result;

	result = bs_encode_size(bs, "notanencoding");
	fail_unless(result == 0);

	bs_free(bs);
}
END_TEST

START_TEST(test_encode_bad_encoding)
{
	BS *bs = bs_create();
	char *output = (char *) 0xDEADBEEF;
	BSresult result;

	result = bs_encode(bs, "notanencoding", output);
	fail_unless(result == BS_BAD_ENCODING);

	bs_free(bs);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Encodings");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_test(tc_core, test_decode_bad_encoding);
	tcase_add_test(tc_core, test_encode_size_bad_encoding);
	tcase_add_test(tc_core, test_encode_bad_encoding);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
