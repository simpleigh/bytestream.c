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
	size_t size;
	BSresult result;

	result = bs_encode_size(bs, "notanencoding", &size);
	fail_unless(result == BS_BAD_ENCODING);

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

struct BSEncodingTestcase {
	const char *szEncoding;
	const char *szInput;
	size_t cchInput;
	BSbyte *rgbBytes;
	size_t cbBytes;
	const char *szOutput;
	size_t cchOutput;
};

struct BSEncodingTestcase
rgTestcases[] = {
	{ "base64",    "FPucA9l+",         8,  "\x14\xfb\x9c\x03\xd9\x7e", 6, "FPucA9l+",         9  },
	{ "base64",    "FPucA9k=",         8,  "\x14\xfb\x9c\x03\xd9",     5, "FPucA9k=",         9  },
	{ "base64",    "FPucAw==",         8,  "\x14\xfb\x9c\x03",         4, "FPucAw==",         9  },

	/* These next testcases from RFC 4648 */
	{ "base64",    "",                 0,  "",                         0, "",                 1  },
	{ "base64",    "Zg==",             4,  "f",                        1, "Zg==",             5  },
	{ "base64",    "Zm8=",             4,  "fo",                       2, "Zm8=",             5  },
	{ "base64",    "Zm9v",             4,  "foo",                      3, "Zm9v",             5  },
	{ "base64",    "Zm9vYg==",         8,  "foob",                     4, "Zm9vYg==",         9  },
	{ "base64",    "Zm9vYmE=",         8,  "fooba",                    5, "Zm9vYmE=",         9  },
	{ "base64",    "Zm9vYmFy",         8,  "foobar",                   6, "Zm9vYmFy",         9  }/*,
	{ "base32",    "",                 0,  "",                         0, "",                 1  },
	{ "base32",    "MY======",         8,  "f",                        1, "MY======",         9  },
	{ "base32",    "MZXQ====",         8,  "fo",                       2, "MZXQ====",         9  },
	{ "base32",    "MZXW6===",         8,  "foo",                      3, "MZXW6===",         9  },
	{ "base32",    "MZXW6YQ=",         8,  "foob",                     4, "MZXW6YQ=",         9  },
	{ "base32",    "MZXW6YTB",         8,  "fooba",                    5, "MZXW6YTB",         9  },
	{ "base32",    "MZXW6YTBOI======", 16, "foobar",                   6, "MZXW6YTBOI======", 17 },
	{ "base32hex", "",                 0,  "",                         0, "",                 1  },
	{ "base32hex", "CO======",         8,  "f",                        1, "CO======",         9  },
	{ "base32hex", "CPNG====",         8,  "fo",                       2, "CPNG====",         9  },
	{ "base32hex", "CPNMU===",         8,  "foo",                      3, "CPNMU===",         9  },
	{ "base32hex", "CPNMUOG=",         8,  "foob",                     4, "CPNMUOG=",         9  },
	{ "base32hex", "CPNMUOJ1",         8,  "fooba",                    5, "CPNMUOJ1",         9  },
	{ "base32hex", "CPNMUOJ1E8======", 16, "foobar",                   6, "CPNMUOJ1E8======", 17 },
	{ "base16",    "",                 0,  "",                         0, "",                 1  },
	{ "base16",    "66",               2,  "f",                        1, "66",               3  },
	{ "base16",    "666F",             4,  "fo",                       2, "666F",             5  },
	{ "base16",    "666F6F",           6,  "foo",                      3, "666F6F",           7  },
	{ "base16",    "666F6F62",         8,  "foob",                     4, "666F6F62",         9  },
	{ "base16",    "666F6F6261",       10, "fooba",                    5, "666F6F6261",       11 },
	{ "base16",    "666F6F626172",     12, "foobar",                   6, "666F6F626172",     13 }, */
};

START_TEST(test_decode)
{
	struct BSEncodingTestcase testcase = rgTestcases[_i];
	BS *bs = bs_create();
	BSresult result;

	result = bs_decode(
		bs,
		testcase.szEncoding,
		testcase.szInput,
		testcase.cchInput
	);
	fail_unless(result == BS_OK);
	fail_unless(bs_size(bs) == testcase.cbBytes);
	fail_unless(
		memcmp(bs_get_buffer(bs), testcase.rgbBytes, testcase.cbBytes) == 0
	);

	bs_free(bs);
}
END_TEST

START_TEST(test_encode_size)
{
	struct BSEncodingTestcase testcase = rgTestcases[_i];
	BS *bs = bs_create();
	size_t cchOutput;
	BSresult result;

	bs_set_buffer(bs, testcase.rgbBytes, testcase.cbBytes);

	result = bs_encode_size(bs, testcase.szEncoding, &cchOutput);
	fail_unless(result == BS_OK);
	fail_unless(cchOutput == testcase.cchOutput);

	bs_unset_buffer(bs);
	bs_free(bs);
}
END_TEST

START_TEST(test_encode)
{
	struct BSEncodingTestcase testcase = rgTestcases[_i];
	BS *bs = bs_create();
	size_t cchOutput;
	char *szOutput;
	BSresult result;

	bs_set_buffer(bs, testcase.rgbBytes, testcase.cbBytes);

	result = bs_encode_size(bs, testcase.szEncoding, &cchOutput);
	fail_unless(result == BS_OK);

	szOutput = malloc(cchOutput);
	fail_unless(szOutput != NULL);

	result = bs_encode(bs, testcase.szEncoding, szOutput);
	fail_unless(result == BS_OK);
	fail_unless(memcmp(szOutput, testcase.szOutput, testcase.cchOutput) == 0);

	bs_unset_buffer(bs);
	bs_free(bs);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Encodings");
	TCase *tc_core = tcase_create("Core");
	size_t cTestcases = sizeof(rgTestcases) / sizeof(struct BSEncodingTestcase);
	SRunner *sr;
	int number_failed;

	tcase_add_test(tc_core, test_decode_bad_encoding);
	tcase_add_test(tc_core, test_encode_size_bad_encoding);
	tcase_add_test(tc_core, test_encode_bad_encoding);

	tcase_add_loop_test(tc_core, test_decode, 0, cTestcases);
	tcase_add_loop_test(tc_core, test_encode_size, 0, cTestcases);
	tcase_add_loop_test(tc_core, test_encode, 0, cTestcases);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}