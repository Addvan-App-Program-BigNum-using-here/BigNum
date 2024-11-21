#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
// 윈도우일 때는 time.h 인 것으로 판단
#include <time.h>
#include "../util.h"
#include "../msg_control.h"
#include "../random.h"
#include "../operate.h"
#include "../data_type.h"
#include "../file_io.h"

#define test_word_size  0      // 0인 경우 테스트 시 사용되는 word 크기는 랜덤
#define test_word_size_limit 100 // 랜덤 word 크기 사용 시 제한 범위
#define test_size  100         // 테스트 횟수

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
msg test_bi_set_from_base(const IN int base);

/**
 * @brief Test bigint structure set random
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_random();

/**
 * @brief Test bigint structure Add operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_add();

/**
 * @brief Test bigint structure Sub operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_sub();

/**
 * @brief Test bigint structure Mul operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_mul();

/**
 * @brief Test bigint structure Mul operation using Karachuba algorithm
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_mul_karachuba();

/**
 * @brief Test multiplication classic and Karachuba algorithm same value
 *
 * @param start_size start size of bigint
 * @param end_size end size of bigint
 * @param step_size step size of bigint
 * @return message SUCCESS or FAIL
 */
msg compare_multiplicaiton(int start_size, int end_size, int step_size);

msg test_bi_div();

msg test_bi_shift();

msg test_bi_get_lower();

msg test_bi_cat();
#endif // UTIL_TEST_H