#ifndef TEST_MAIN_H
#define TEST_MAIN_H

#define BITS_SIZE 6                             // 테스트 비트 크기
#define test_word_size BITS_SIZE / WORD_BITS               // 0인 경우 테스트 시 사용되는 word 크기는 랜덤
#define test_word_size_limit BITS_SIZE / WORD_BITS          // 랜덤 word 크기 사용 시 제한 범위
#define test_size  10                                   // 테스트 횟수
#define barret_word_size test_word_size                // barret N 크기

#include <stdio.h>

#include "util_test.h"
#include "operate_test.h"
#include "crypto_test.h"
#include "random_test.h"

/**
 * @brief Clear test file
 *
 * @return message SUCCESS or FAIL
 */
msg CLEAR_Test_file();

/**
 * @brief print data set
 *
 * @return message SUCCESS or FAIL
 */
void print_data_set();

#endif // TEST_MAIN_H