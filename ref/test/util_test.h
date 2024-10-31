#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include "../util.h"
#include "../msg_control.h"
#include "../random.h"
#include "../operate.h"

/**
 * @brief Test bigint structure allocation and deallocation
 *
 * @return int
 */
msg test_bi_new_delete();

/**
 * @brief Test bigint structure set content from array and string
 *
 * @return int
 */
msg test_bi_set_from();

/**
 * @brief Test bigint structure set random
 *
 * @return msg
 */
msg test_bi_random();

/**
 * @brief Test bigint structure operation
 *
 * @return msg
 */
msg test_bi_operate();


#endif // UTIL_TEST_H