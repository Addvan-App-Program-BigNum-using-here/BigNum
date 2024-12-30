#ifndef BENCH_UTIL_H
#define BENCH_UTIL_H

#include <stdio.h>
#include <unistd.h>
#include "util.h"
#include "operate.h"
#include "msg_control.h"
#include "random.h"
#include "data_type.h"
#include "crypto.h"

#define TEST_LOOP 10                            // 테스트 루프 횟수
#define RANDOM_TEST_WORD_SIZE 0
#define BITS_SIZE 1024                              // 테스트 비트 크기
#define test_word_size BITS_SIZE / WORD_BITS                // 0인 경우 테스트 시 사용되는 word 크기는 랜덤
#define test_word_size_limit BITS_SIZE / WORD_BITS          // 랜덤 word 크기 사용 시 제한 범위 -> 여기 나중에 limit라서 0 ~ limit - 1인거 가만해서 수정 필요
#define test_size  1                                   // 테스트 횟수
#define barret_word_size test_word_size                // barret N 크기
/**
 * @brief print data set
 *
 * @return message SUCCESS or FAIL
 */
void print_data_set();

/**
 * @brief print progress bar
 * @param percentage percentage of progress
 * @param width width of progress bar
 */
void printProgressBar(float percentage, int width);
#endif // BENCH_UTIL_H