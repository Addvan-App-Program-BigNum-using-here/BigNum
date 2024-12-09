#ifndef FUNC_CMP_TEST_H
#define FUNC_CMP_TEST_H

#include "../data_type.h"
#include "../msg_control.h"
#include "../util.h"
#include "../operate.h"
#include "test_main.h"

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
msg compare_exponention(int start_size, int end_size, int step_size);

msg func_compare();

#endif // FUNC_CMP_TEST_H