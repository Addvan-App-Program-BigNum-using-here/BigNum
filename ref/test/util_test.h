#ifndef UTIL_TEST_H
#define UTIL_TEST_H

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

#define BITS_SIZE 2048                              // 테스트 비트 크기
#define test_word_size BITS_SIZE / WORD_BITS               // 0인 경우 테스트 시 사용되는 word 크기는 랜덤
#define test_word_size_limit BITS_SIZE / WORD_BITS          // 랜덤 word 크기 사용 시 제한 범위
#define test_size  10                                   // 테스트 횟수
#define barret_word_size test_word_size                // barret N 크기

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
 * @brief Test bigint structure Add operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_add(OUT double* total_time_add, IN bigint** a, IN bigint** b, IN char* str);

/**
 * @brief Test bigint structure Sub operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_sub(OUT double* total_time_sub, IN bigint** a, IN bigint** b, IN char* str);

/**
 * @brief Test bigint structure Mul operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_mul(OUT double* total_time_mul, IN bigint** a, IN bigint** b, IN char* str);

/**
 * @brief Test bigint structure Mul operation using Karachuba algorithm
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_mul_karachuba(OUT double* total_time_mul_karachuba, IN bigint** a, IN bigint** b, IN char* str, IN int* karachuba_flag);

/**
 * @brief Test classic and Karachuba algorithm same value
 *
 * @param start_size start size of bigint
 * @param end_size end size of bigint
 * @param step_size step size of bigint
 * @return message SUCCESS or FAIL
 */
msg compare_multiplicaiton(int start_size, int end_size, int step_size);
msg compare_squaring(int start_size, int end_size, int step_size);
msg compare_division(int start_size, int end_size, int step_size);

/**
 * @brief Test bigint structure Division operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_div(OUT double* total_time_div, IN bigint** a, IN bigint** b, IN char* str, IN int* option);

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
 * @brief Test bigint structure Squaring operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_squ(OUT double* total_time_squ, IN bigint** a, IN char* str);

/**
 * @brief Test bigint structure Squaring operation by using Karachuba algorithm
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_squ_karachuba(OUT double* total_time_squ_karachuba, IN bigint** a, IN char* str, IN int* squ_karachuba_flag);

/**
 * @brief Test bigint structure Exponentional operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_exp(OUT double total_time_exp[3], IN bigint** a, IN bigint** b, IN bigint** c, IN char* str);

/**
 * @brief Test bigint structure Barret Reduction operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_barrett_reduction(OUT double* total_time_barret_reduction, IN bigint** a, IN bigint** barret_N, IN bigint** barret_T, IN char* str);

msg test_bi_gcd(OUT double* total_time_div, IN bigint** a, IN bigint** b, IN char* str);
#endif // UTIL_TEST_H