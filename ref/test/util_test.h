#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "../util.h"
#include "../msg_control.h"
#include "../random.h"
#include "../operate.h"
#include "../data_type.h"
#include "../file_io.h"

// popen에서 나오는 출력 값 예외처리
// Windows 시스템
#ifdef _WIN32
#define PYTHON_REDIRECT " >nul 2>nul"
#define PYTHON_CMD "python "
// Unix/Linux/MacOS 시스템
#else
#define PYTHON_REDIRECT " >/dev/null 2>&1"
#define PYTHON_CMD "python3 "
#endif

/**
 * @brief Test bigint structure allocation and deallocation
 *
 * @param test_size size of test
 * @param test_word_size size of bigint struct
 * @return message SUCCESS or FAIL
 */
msg test_bi_new_delete(const IN int test_size, const IN int test_word_size);

/**
 * @brief Test bigint structure set content from array and string
 *
 * @param test_size size of test
 * @param test_word_size size of bigint struct
 * @return message SUCCESS or FAIL
 */
msg test_bi_set_from(const IN int test_size, const IN int test_word_size);

/**
 * @brief Test bigint structure set content from array and string in each base
 *
 * @param test_size size of test
 * @param test_word_size size of bigint struct
 * @return message SUCCESS or FAIL
 */
msg test_bi_set_from_base(const IN int test_size, const IN int base);

/**
 * @brief Test bigint structure set random
 *
 * @param test_size size of test
 * @param test_word_size size of bigint struct
 * @return message SUCCESS or FAIL
 */
msg test_bi_random(const IN int test_size, const IN int test_word_size);

/**
 * @brief Test bigint structure Add operation
 *
 * @param test_size size of test
 * @param test_word_size size of bigint struct
 * @return message SUCCESS or FAIL
 */
msg test_bi_add(const IN int test_size, const IN int test_word_size);

/**
 * @brief Test bigint structure Sub operation
 *
 * @param test_size size of test
 * @param test_word_size size of bigint struct
 * @return message SUCCESS or FAIL
 */
msg test_bi_sub(const IN int test_size, const IN int test_word_size);

/**
 * @brief Test bigint structure Mul operation
 *
 * @param test_size size of test
 * @param test_word_size size of bigint struct
 * @return message SUCCESS or FAIL
 */
msg test_bi_mul(const IN int test_size, const IN int test_word_size);

/**
 * @brief Test bigint structure Mul operation using Karachuba algorithm
 *
 * @param test_size size of test
 * @param test_word_size size of bigint struct
 * @return message SUCCESS or FAIL
 */
msg test_bi_mul_karachuba(const IN int test_size, const IN int test_word_size);

/**
 * @brief Test multiplication classic and Karachuba algorithm same value
 *
 * @param test_size size of test
 * @param test_word_size size of bigint struct
 * @return message SUCCESS or FAIL
 */
msg compare_multiplicaiton(int start_size, int end_size, int step_size, int iterations);

msg test_bi_div(const IN int test_size, const IN int test_word_size);
#endif // UTIL_TEST_H