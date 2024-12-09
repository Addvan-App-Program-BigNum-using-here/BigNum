#ifndef CRYPTO_TEST_H
#define CRYPTO_TEST_H

#include "../data_type.h"
#include "../msg_control.h"
#include "../util.h"
#include "util_test.h"
#include "test_main.h"

/**
 * @brief Test bigint structure crypto by using fixed bigint
 *
 * @return message SUCCESS or FAIL
 */
msg cmp_crypto_test();

/**
 * @brief Test bigint structure crypto by using random bigint
 *
 * @return message SUCCESS or FAIL
 */
msg rand_crypto_test();

/**
 * @brief Test bigint structure crypto onetime
 *
 * @return message SUCCESS or FAIL
 */
msg crypto_test(IN bigint** a, IN bigint** b);
 
/**
 * @brief Test RSA operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_RSA(OUT double* total_time_RSA, IN bigint** a, IN char* str);

/**
 * @brief Test RSA CRT operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_RSA_CRT(OUT double* total_time_RSA_CRT, IN bigint** a, IN char* str);

/**
 * @brief Test bigint structure GCD operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_gcd(OUT double* total_time_div, IN bigint** a, IN bigint** b, IN char* str);

/**
 * @brief Test bigint structure Extended Eucliean Algorithm operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_bi_EEA(OUT double* total_time_div, IN bigint** a, IN bigint** b, IN char* str);


/**
 * @brief Test bigint structure Miller Rabin operation
 *
 * @return message SUCCESS or FAIL
 */
msg test_miller_rabin(double* total_time_MR , IN bigint** a, IN int* iteration, char* str);

/**
 * @brief Clear test file
 *
 * @return message SUCCESS or FAIL
 */
msg CLEAR_CRYPTO_Test_file();

/**
 * @brief Print result
 *
 * @return message SUCCESS or FAIL
 */
void crypto_print_result();

#endif // CRYPTO_TEST_H