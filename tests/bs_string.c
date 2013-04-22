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

START_TEST(test_hex_load_bad_length)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_load_hex(bs, "123", 3);
	fail_unless(result == BS_INVALID);

	bs_free(bs);
}
END_TEST

START_TEST(test_hex_load_bad_character)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_load_hex(bs, "123#", 4);
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

START_TEST(test_hex_load)
{
	BS *bs = bs_create();
	size_t cbInput, cbOutput, ibOutput;
	BSresult result;

	cbInput = strlen(test_hex_in);
	cbOutput = cbInput >> 1;
	result = bs_load_hex(bs, test_hex_in, cbInput);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == cbOutput);

	for (ibOutput = 0; ibOutput < cbOutput; ibOutput++) {
		fail_unless(bs_get_byte(bs, ibOutput) == test_hex_out[ibOutput]);
	}

	bs_free(bs);
}
END_TEST

static const char
test_hex_return[] = "0123456789abcdefabcdef";

START_TEST(test_hex_save)
{
	BS *bs = bs_create();
	size_t cbString = strlen(test_hex_in);
	char *hex;
	size_t length;
	BSresult result;

	bs_load_hex(bs, test_hex_in, cbString);

	result = bs_save_hex(bs, &hex, &length);
	fail_unless(result == BS_OK);
	fail_unless(hex != NULL);
	fail_unless(length == cbString);
	fail_unless(hex[cbString] == '\0');
	fail_unless(strlen(hex) == cbString);
	fail_unless(strcmp(hex, test_hex_return) == 0);

	free(hex);
	bs_free(bs);
}
END_TEST

struct base64_testcase_struct {
	BSbyte rgBytes[3];
	size_t cbBytes;
	char base64[5];
};

static const struct base64_testcase_struct
base64_testcases[11] = {
	{ {   0,   0,   0 }, 1, "AA==" },
	{ {   0,   0,   0 }, 2, "AAA=" },
	{ {   0,   0,   0 }, 3, "AAAA" },
	{ { 255, 255, 255 }, 1, "/w==" },
	{ { 255, 255, 255 }, 2, "//8=" },
	{ { 255, 255, 255 }, 3, "////" },
	{ {   0,   0,  46 }, 3, "AAAu" },
	{ { 'M', 'a', 'n' }, 3, "TWFu" },
	{ { 'a', 'n', 'y' }, 3, "YW55" },
	{ { 151,  79, 191 }, 3, "l0+/" },
	{ {   0,  16, 131 }, 3, "ABCD" },
};

START_TEST(test_b64_load)
{
	BS *bs = bs_create();
	const char *base64 = base64_testcases[_i].base64;
	BSresult result;
	size_t ibByte;

	result = bs_load_base64(bs, base64, 4);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == base64_testcases[_i].cbBytes);

	for (ibByte = 0; ibByte < base64_testcases[_i].cbBytes; ibByte++) {
		fail_unless(
			bs_get_byte(bs, ibByte) == base64_testcases[_i].rgBytes[ibByte]
		);
	}

	bs_free(bs);
}
END_TEST

START_TEST(test_b64_save)
{
	BS *bs = bs_create();
	size_t cbString = 4;
	char *base64;
	size_t length;
	BSresult result;

	bs_load_base64(bs, base64_testcases[_i].base64, cbString);

	result = bs_save_base64(bs, &base64, &length);
	fail_unless(result == BS_OK);
	fail_unless(base64 != NULL);
	fail_unless(length == cbString);
	fail_unless(base64[cbString] == '\0');
	fail_unless(strlen(base64) == cbString);
	fail_unless(strcmp(base64, base64_testcases[_i].base64) == 0);

	free(base64);
	bs_free(bs);
}
END_TEST

START_TEST(test_b64_load_many)
{
	BS *bs = bs_create();
	BSresult result;
	BSbyte expected[] = "easure.";
	size_t ibByte;

	result = bs_load_base64(bs, "ZWFzdXJlLg==", 12);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == 7);

	for (ibByte = 0; ibByte < 7; ibByte++) {
		fail_unless(bs_get_byte(bs, ibByte) == expected[ibByte]);
	}

	bs_free(bs);
}
END_TEST

START_TEST(test_b64_save_many)
{
	BS *bs = bs_create();
	char *base64;
	size_t length;
	BSresult result;
	char expected[] = "ZWFzdXJlLg==";

	bs_load_base64(bs, expected, 12);

	result = bs_save_base64(bs, &base64, &length);
	fail_unless(result == BS_OK);
	fail_unless(base64 != NULL);
	fail_unless(length == 12);
	fail_unless(base64[12] == '\0');
	fail_unless(strlen(base64) == 12);
	fail_unless(strcmp(base64, expected) == 0);

	free(base64);
	bs_free(bs);
}
END_TEST

START_TEST(test_b64_bad_length)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_load_base64(bs, "123", 3);
	fail_unless(result == BS_INVALID);
}
END_TEST

static const char
bad_chars[6] = { '!', '"', '#', '\0', 127, 255 };

START_TEST(test_b64_bad_chars)
{
	BS *bs = bs_create();
	char base64[] = "A*==";
	BSresult result;

	base64[1] = bad_chars[_i];
	result = bs_load_base64(bs, base64, 4);
	fail_unless(result == BS_INVALID);

	bs_free(bs);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Strings");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_test(tc_core, test_load);
	tcase_add_test(tc_core, test_load_empty);
	tcase_add_test(tc_core, test_save);
	tcase_add_test(tc_core, test_hex_load_bad_length);
	tcase_add_test(tc_core, test_hex_load_bad_character);
	tcase_add_test(tc_core, test_hex_load);
	tcase_add_test(tc_core, test_hex_save);
	tcase_add_loop_test(tc_core, test_b64_load, 0, 11);
	tcase_add_loop_test(tc_core, test_b64_save, 0, 11);
	tcase_add_test(tc_core, test_b64_load_many);
	tcase_add_test(tc_core, test_b64_save_many);
	tcase_add_test(tc_core, test_b64_bad_length);
	tcase_add_loop_test(tc_core, test_b64_bad_chars, 0, 6);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
