#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
 * @return int
 */
msg test_bi_new_delete();

/**
 * @brief Test bigint structure set content from array and string
 *
 * @return int
 */
msg test_bi_set_from(int test_size, int test_word_size);

/**
 * @brief Test bigint structure set random
 *
 * @return msg
 */
msg test_bi_random(const IN int test_size, const IN int test_word_size);

/**
 * @brief Test bigint structure Add operation
 *
 * @return msg
 */
msg test_bi_add(const IN int test_size, const IN int test_word_size);

/**
 * @brief Test bigint structure Sub operation
 *
 * @return msg
 */
msg test_bi_sub(const IN int test_size, const IN int test_word_size);

/**
 * @brief Test bigint structure Mul operation
 *
 * @return msg
 */
msg test_bi_mul();

#endif // UTIL_TEST_H