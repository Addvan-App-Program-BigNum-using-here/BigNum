#ifndef OPERATE_TEST_H
#define OPERATE_TEST_H

#include "util.h"
#include "msg_control.h"
#include "random.h"
#include "operate.h"
#include "data_type.h"
#include "file_io.h"
#include "random_test.h"
#include "util_test.h"
#include "test_main.h"

/**
 * @brief Test bigint structure operate by using fixed bigint
 *
 * @return message SUCCESS or FAIL
 */
msg cmp_operate_test();

/**
 * @brief Test bigint structure operate by using random bigint
 *
 * @return message SUCCESS or FAIL
 */
msg rand_operate_test();

/**
 * @brief Test bigint structure operate onetime
 *
 * @return message SUCCESS or FAIL
 */
msg operate_test(IN bigint** a, IN bigint** b, IN bigint** c, IN bigint** barret_N, IN bigint** barret_T);

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
 * @brief Test bigint structure Division operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_div(OUT double* total_time_div, IN bigint** a, IN bigint** b, IN char* str, IN int* option);

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

/**
 * @brief Print result
 *
 * @return message SUCCESS or FAIL
 */
void operate_print_result();
#endif // OPERATE_TEST_H