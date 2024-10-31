#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include <stdio.h>
#include "../util.h"
#include "../msg_control.h"

/**
 * @brief Test bigint structure allocation and deallocation
 *
 * @return int
 */
int test_bi_new_delete();

/**
 * @brief Test bigint structure set content from array and string
 *
 * @return int
 */
int test_bi_set_from();

/**
 * @brief Test bigint structure set random
 *
 * @return msg
 */
msg test_bi_random();


#endif // UTIL_TEST_H