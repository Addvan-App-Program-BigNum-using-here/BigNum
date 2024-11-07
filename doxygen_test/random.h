#include "error_msg.h"
#include "util.h"
#include <stdio.h>

/**
 * @brief New allocate bigint struct and fill it with random values
 * 
 * @param dst pointer to bigint struct
 * @param word_len length of bigint struct
 * @todo rand() 함수 DRBG로 교체 필요, array_rand, array_rand_ver2 중 어떤걸 사용할지 선택 필요
 */
msg bi_get_random(bigint** dst, int word_len);

/**
 * @brief Fill word array with random values using for bi_get_random()
 * 
 * @param dst pointer to word array
 * @param word_len length of bigint struct
 * @todo rand() 함수 DRBG로 교체 필요
 */
msg array_rand(word* dst, int word_len);

/**
 * @brief Fill word array with random values using for bi_get_random()
 * 
 * @param dst pointer to word array
 * @param word_len length of bigint struct
 * @todo rand() 함수 DRBG로 교체 필요
 */
msg array_rand_ver2(word* dst, int word_len);