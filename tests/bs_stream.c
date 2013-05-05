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

#define UNUSED(x) (void)(x)

struct operation_data {
	unsigned int cCalls;
	unsigned int cbWritten;
	char szData[31];
};

static BSresult
operation(const BS *bs, void *data)
{
	struct operation_data *test_data = (struct operation_data *) data;
	char *stream;
	size_t length;

	test_data->cCalls++;
	test_data->cbWritten += bs_size(bs);

	stream = malloc(bs_size(bs) + 1);
	fail_unless(stream != NULL);
	stream[bs_size(bs)] = '\0';

	bs_save(bs, (BSbyte **) stream);
	strcat(test_data->szData, stream);

	return BS_OK;
}

static BSbyte *
stream = (BSbyte *) "1234567890";

START_TEST(test_stream)
{
	struct operation_data data = { 0, 0, "" };
	BS *bs = bs_create_size(5);
	BSresult result;

	result = bs_stream(bs, stream, 10, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 2);
	fail_unless(data.cbWritten == 10);

	result = bs_stream(bs, stream, 5, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 3);
	fail_unless(data.cbWritten == 15);

	result = bs_stream(bs, stream, 2, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 3);
	fail_unless(data.cbWritten == 15);

	result = bs_stream(bs, stream, 2, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 3);
	fail_unless(data.cbWritten == 15);

	result = bs_stream(bs, stream, 0, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 3);
	fail_unless(data.cbWritten == 15);

	result = bs_stream(bs, stream, 2, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 4);
	fail_unless(data.cbWritten == 20);

	result = bs_stream(bs, stream, 6, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 5);
	fail_unless(data.cbWritten == 25);

	result = bs_stream(bs, stream, 3, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 6);
	fail_unless(data.cbWritten == 30);

	fail_unless(strcmp(data.szData, "123456789012345121212123456123") == 0);

	bs_free(bs);
}
END_TEST

static BSresult
operation_invalid(const BS *bs, void *data)
{
	UNUSED(data);

	return 999;
}

START_TEST(test_stream_bad_operation)
{
	BS *bs = bs_create_size(5);
	BSresult result;

	result = bs_stream(bs, stream, 5, operation_invalid, NULL);
	fail_unless(result == 999);

	result = bs_stream(bs, stream, 2, operation_invalid, NULL);
	fail_unless(result == BS_OK);

	result = bs_stream(bs, stream, 3, operation_invalid, NULL);
	fail_unless(result == 999);

	bs_free(bs);
}
END_TEST

START_TEST(test_stream_empty_bs)
{
	BS *bs = bs_create();
	BSresult result;

	result = bs_stream(bs, stream, 5, operation, NULL);
	fail_unless(result == BS_INVALID);

	bs_free(bs);
}
END_TEST

START_TEST(test_stream_null_bs)
{
	BSresult result;

	result = bs_stream(NULL, stream, 5, operation, NULL);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_stream_null_data)
{
	BS *bs = bs_create_size(5);
	BSresult result;

	result = bs_stream(bs, NULL, 5, operation, NULL);
	fail_unless(result == BS_NULL);

	bs_free(bs);
}
END_TEST

START_TEST(test_flush)
{
	struct operation_data data = { 0, 0, "" };
	BS *bs = bs_create_size(5);
	BSresult result;

	result = bs_stream(bs, stream, 2, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 0);
	fail_unless(data.cbWritten == 0);

	result = bs_stream_flush(bs, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 1);
	fail_unless(data.cbWritten == 2);

	result = bs_stream_flush(bs, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 1);
	fail_unless(data.cbWritten == 2);

	result = bs_stream(bs, stream, 4, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 1);
	fail_unless(data.cbWritten == 2);

	result = bs_stream_flush(bs, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 2);
	fail_unless(data.cbWritten == 6);

	fail_unless(strcmp(data.szData, "121234") == 0);

	bs_free(bs);
}
END_TEST

START_TEST(test_flush_null_bs)
{
	BSresult result;

	result = bs_stream_flush(NULL, operation, NULL);
	fail_unless(result == BS_NULL);
}
END_TEST

START_TEST(test_reset)
{
	struct operation_data data = { 0, 0, "" };
	BS *bs = bs_create_size(5);
	BSresult result;

	result = bs_stream(bs, stream, 2, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 0);
	fail_unless(data.cbWritten == 0);

	bs_stream_reset(bs);
	fail_unless(data.cCalls == 0);
	fail_unless(data.cbWritten == 0);

	result = bs_stream(bs, stream, 4, operation, &data);
	fail_unless(result == BS_OK);
	fail_unless(data.cCalls == 0);
	fail_unless(data.cbWritten == 0);

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
	tcase_add_test(tc_core, test_reset);

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
