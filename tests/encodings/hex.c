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
#include <string.h>

START_TEST(test_load_bad_length)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_decode(bs, "hex", "123", 3);
	fail_unless(result == BS_INVALID);

	bs_free(bs);
}
END_TEST

START_TEST(test_load_bad_character)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_decode(bs, "hex", "123#", 4);
	fail_unless(result == BS_INVALID);

	bs_free(bs);
}
END_TEST

static const char
test_hex_in[] = "0123456789abcdefABCDEF";

static const BSbyte
test_hex_out[] = {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xAB, 0xCD, 0xEF
};

START_TEST(test_load)
{
	BS *bs = bs_create();
	size_t cbInput, cbOutput, ibOutput;
	BSresult result;

	cbInput = strlen(test_hex_in);
	cbOutput = cbInput >> 1;
	result = bs_decode(bs, "hex", test_hex_in, cbInput);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == cbOutput);

	for (ibOutput = 0; ibOutput < cbOutput; ibOutput++) {
		fail_unless(bs_get_byte(bs, ibOutput) == test_hex_out[ibOutput]);
	}

	bs_free(bs);
}
END_TEST

START_TEST(test_load_null_bs)
{
	BSbyte *data = (BSbyte *) 0xDEADBEEF;
	BSresult result;

	result = bs_decode(NULL, "hex", data, 5);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_load_null_data)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_decode(bs, "hex", NULL, 5);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST

static const char
test_hex_return[] = "0123456789abcdefabcdef";

START_TEST(test_size)
{
	BS *bs = bs_create();
	size_t cbString = strlen(test_hex_in);
	BSresult result;

	result = bs_decode(bs, "hex", test_hex_in, cbString);
	fail_unless(result == BS_OK);
	fail_unless(bs_encode_size(bs, "hex") == cbString + 1);

	bs_free(bs);
}
END_TEST

START_TEST(test_save)
{
	BS *bs = bs_create();
	size_t cbString = strlen(test_hex_in);
	char *hex;
	BSresult result;

	result = bs_decode(bs, "hex", test_hex_in, cbString);
	fail_unless(result == BS_OK);

	hex = malloc(bs_encode_size(bs, "hex"));
	fail_unless(hex != NULL);

	result = bs_encode(bs, "hex", hex);
	fail_unless(result == BS_OK);
	fail_unless(hex != NULL);
	fail_unless(strlen(hex) == cbString);
	fail_unless(strcmp(hex, test_hex_return) == 0);

	free(hex);
	bs_free(bs);
}
END_TEST

START_TEST(test_save_null_bs)
{
	char *data = (char *) 0xDEADBEEF;
	BSresult result;

	result = bs_encode(NULL, "hex", data);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_save_null_hex)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_encode(bs, "hex", NULL);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Hex");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_test(tc_core, test_load_bad_length);
	tcase_add_test(tc_core, test_load_bad_character);
	tcase_add_test(tc_core, test_load);
	tcase_add_test(tc_core, test_load_null_bs);
	tcase_add_test(tc_core, test_load_null_data);
	tcase_add_test(tc_core, test_size);
	tcase_add_test(tc_core, test_save);
	tcase_add_test(tc_core, test_save_null_bs);
	tcase_add_test(tc_core, test_save_null_hex);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}