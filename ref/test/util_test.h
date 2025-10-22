#ifndef UTIL_TEST_H
#define UTIL_TEST_H

extern clock_t c_start;
extern clock_t c_end;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/time.h>
// 윈도우일 때는 time.h 인 것으로 판단
#include <time.h>
#include "../util.h"
#include "../msg_control.h"
#include "../random.h"
#include "../operate.h"
#include "../data_type.h"
#include "../file_io.h"
#include "../crypto.h"
#include "random_test.h"

/**
 * @brief Test bigint structure allocation and deallocation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_new_delete();

/**
 * @brief Test bigint structure set content from array and string
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_set_from();

/**
 * @brief Test bigint structure set content from array and string in each base
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_set_from_base(IN int* base);

/**
 * @brief Test bigint structure set random
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_random();

/**
 * @brief Test bigint structure Shift operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_shift();

/**
 * @brief Test bigint structure get Lower word operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_get_lower();

/**
 * @brief Test bigint structure concatenate operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_cat();

/**
 * @brief Create new bigint test vector
 *
 * @return message SUCCESS or FAIL
 */
msg new_bigint_test_vector();
#endif // UTIL_TEST_H