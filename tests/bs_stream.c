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

static unsigned int
cCalls = 0;

static unsigned int
cbWritten = 0;

static char
szData[31];

static BSresult
operation(const BS *bs)
{
	char *data;
	size_t length;

	cCalls++;
	cbWritten += bs_size(bs);

	bs_save(bs, (BSbyte **) &data, &length);
	strcat(szData, data);

	return BS_OK;
}

static BSbyte *
data = (BSbyte *) "1234567890";

START_TEST(test_stream)
{
	BS *bs = bs_create_size(5);
	BSresult result;

	szData[0] = '\0';

	fail_unless(cCalls == 0);

	result = bs_stream(bs, data, 10, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 2);
	fail_unless(cbWritten == 10);

	result = bs_stream(bs, data, 5, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 3);
	fail_unless(cbWritten == 15);

	result = bs_stream(bs, data, 2, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 3);
	fail_unless(cbWritten == 15);

	result = bs_stream(bs, data, 2, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 3);
	fail_unless(cbWritten == 15);

	result = bs_stream(bs, data, 0, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 3);
	fail_unless(cbWritten == 15);

	result = bs_stream(bs, data, 2, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 4);
	fail_unless(cbWritten == 20);

	result = bs_stream(bs, data, 6, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 5);
	fail_unless(cbWritten == 25);

	result = bs_stream(bs, data, 3, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 6);
	fail_unless(cbWritten == 30);

	fail_unless(strcmp(szData, "123456789012345121212123456123") == 0);

	bs_free(bs);
}
END_TEST

static BSresult
operation_invalid(const BS *bs)
{
	return 999;
}

START_TEST(test_stream_bad_operation)
{
	BS *bs = bs_create_size(5);
	BSresult result;

	result = bs_stream(bs, data, 5, operation_invalid);
	fail_unless(result == 999);

	result = bs_stream(bs, data, 2, operation_invalid);
	fail_unless(result == BS_OK);

	result = bs_stream(bs, data, 3, operation_invalid);
	fail_unless(result == 999);

	bs_free(bs);
}
END_TEST

START_TEST(test_stream_empty_bs)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_stream(bs, data, 5, operation);
	fail_unless(result == BS_INVALID);

	bs_free(bs);
}
END_TEST

START_TEST(test_stream_null_bs)
{
	BSresult result;

	result = bs_stream(NULL, data, 5, operation);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_stream_null_data)
{
	BS *bs = bs_create_size(5);
	BSresult result;

	result = bs_stream(bs, NULL, 5, operation);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST

START_TEST(test_flush)
{
	BS *bs = bs_create_size(5);
	BSresult result;

	fail_unless(cCalls == 0);

	result = bs_stream(bs, data, 2, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 0);
	fail_unless(cbWritten == 0);

	result = bs_stream_flush(bs, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 1);
	fail_unless(cbWritten == 2);

	result = bs_stream_flush(bs, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 1);
	fail_unless(cbWritten == 2);

	result = bs_stream(bs, data, 4, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 1);
	fail_unless(cbWritten == 2);

	result = bs_stream_flush(bs, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 2);
	fail_unless(cbWritten == 6);

	fail_unless(strcmp(szData, "121234") == 0);

	bs_free(bs);
}
END_TEST

START_TEST(test_flush_null_bs)
{
	BSresult result;

	result = bs_stream_flush(NULL, operation);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_empty_count)
{
	BS *bs = bs_create_size(5);
	BSresult result;

	fail_unless(cCalls == 0);

	result = bs_stream(bs, data, 2, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 0);
	fail_unless(cbWritten == 0);

	result = bs_stream_empty(bs, NULL, NULL);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 0);
	fail_unless(cbWritten == 0);

	result = bs_stream(bs, data, 4, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 0);
	fail_unless(cbWritten == 0);

	bs_free(bs);
}
END_TEST

START_TEST(test_empty_data)
{
	BS *bs = bs_create_size(5);
	BSbyte *output;
	size_t length;
	BSresult result;

	fail_unless(cCalls == 0);

	result = bs_stream(bs, data, 2, operation);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 0);

	result = bs_stream_empty(bs, &output, &length);
	fail_unless(result == BS_OK);
	fail_unless(cCalls == 0);
	fail_unless(output[0] == '1');
	fail_unless(output[1] == '2');
	fail_unless(length == 2);

	bs_free(bs);
}
END_TEST

START_TEST(test_empty_no_stream)
{
	BS *bs = bs_create_size(5);
	BSbyte *output;
	size_t length;
	BSresult result;

	result = bs_stream_empty(bs, &output, &length);
	fail_unless(result == BS_OK);
	fail_unless(output == NULL);
	fail_unless(length == 0);

	bs_free(bs);
}
END_TEST

START_TEST(test_empty_null_bs)
{
	BSbyte *output;
	size_t length;
	BSresult result;

	result = bs_stream_empty(NULL, &output, &length);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_empty_null_data)
{
	BS *bs = bs_create_size(5);
	size_t length;
	BSresult result;

	result = bs_stream_empty(bs, NULL, &length);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST

START_TEST(test_empty_null_length)
{
	BS *bs = bs_create_size(5);
	BSbyte *output;
	BSresult result;

	result = bs_stream_empty(bs, &output, NULL);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST

int
main(/* int argc, char **argv */)
{
	Suite *s = suite_create("Streaming");
	TCase *tc_core = tcase_create("Core");
	SRunner *sr;
	int number_failed;

	tcase_add_test(tc_core, test_stream);
	tcase_add_test(tc_core, test_stream_bad_operation);
	tcase_add_test(tc_core, test_stream_empty_bs);
	tcase_add_test(tc_core, test_stream_null_bs);
	tcase_add_test(tc_core, test_stream_null_data);
	tcase_add_test(tc_core, test_flush);
	tcase_add_test(tc_core, test_flush_null_bs);
	tcase_add_test(tc_core, test_empty_count);
	tcase_add_test(tc_core, test_empty_data);
	tcase_add_test(tc_core, test_empty_no_stream);
	tcase_add_test(tc_core, test_empty_null_bs);
	tcase_add_test(tc_core, test_empty_null_data);
	tcase_add_test(tc_core, test_empty_null_length);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
