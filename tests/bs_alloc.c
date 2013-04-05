#include "bs.h"
#include <check.h>
#include <stdlib.h>

START_TEST(test_create)
{
	BS *bs = bs_create();

	fail_unless(bs_size(bs) == 0);
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

	suite_add_tcase(s, tc_core);
	sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
