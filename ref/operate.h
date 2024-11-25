#ifndef OPERATE_H
#define OPERATE_H

#include "data_type.h"
#include "msg_control.h"
#include "util.h"

#define flag 10  // karachuba에서 base case 수행 시 일반 곱셈의 기준 6이 가장 높은 듯??
#define MAX_RECURSION_DEPTH 32 // 재귀 깊이
#define POOL_SIZE 8            // 카라츄바에 필요한 임시 변수 개수 (a_0, b_0, a_1, b_1, a_0b_0, a_1b_1, a_1_a_0, b_1_b_0)

// 메모리 풀 구조체
typedef struct {
    bigint* pool[MAX_RECURSION_DEPTH][POOL_SIZE];
    int current_depth;
    int max_word_size;  // 최대 워드 크기 저장
    bool initialized;
} karatsuba_mem_pool;

// 전역 메모리 풀 선언
static karatsuba_mem_pool g_pool = {0};

/**
 * @brief bigint structure add operation
 * @param dst pointer to result of bigint add
 * @param a bigint structure a
 * @param b bigint structure b
 * @param option option for clear or append
 * @return msg
 */
msg bi_add(OUT bigint **dst, IN bigint **a, IN bigint **b);

/**
 * @brief bigint structure sub operation
 * @param dst pointer to result of bigint sub
 * @param a bigint structure a
 * @param b bigint structure b
 * @param option option for clear or append
 * @return msg
 */
msg bi_sub(OUT bigint **dst, IN bigint **a, IN bigint **b);

// ================ 2024-11-11 수정 ========================
/**
 * @brief bigint structure multiplication operation
 * @param dst pointer to result of bigint multiplication
 * @param a first bigint operand
 * @param b second bigint operand
 */
msg bi_mul(OUT bigint **dst, IN bigint **a, IN bigint **b);

/**
 * @brief bigint structure mul operation
 * @param dst pointer to result of bigint mul
 * @param a bigint structure a
 * @param b bigint structure b
 * @return msg
 */
msg bi_mul(OUT bigint **dst, IN bigint **a, IN bigint **b);

/**
 * @brief bigint structure mul operation using Karachuba algorithm
 * @param dst pointer to result of bigint mul
 * @param a bigint structure a
 * @param b bigint structure b
 * @return msg
 */
msg bi_mul_karachuba(OUT bigint **dst, IN bigint **a, IN bigint **b);

/**
 * @brief initialize karachuba memory pool using to karachuba multiplication
 * @param max_word_size maximum word size of bigint struct
 * @return msg
 */
msg init_karachuba_pool(int max_word_size);

/**
 * @brief clear karachuba memory pool
 * @return msg
 */
msg clear_karachuba_pool();

/**
 * @brief bigint structure division operation
 * @param q pointer to quotient of bigint division
 * @param r pointer to remainder of bigint division
 * @param a dividend bigint operand
 * @param b divisor bigint operand
 * @return msg
 */

msg bi_div(OUT bigint **q, OUT bigint **r, IN bigint **a, IN bigint **b);


/**
 * @brief bigint structure sqaure operation
 * @param dst pointer to result of bigint square
 * @param a bigint structure a
 * @return msg
 */
msg bi_squ(OUT bigint** dst, IN bigint** a);

#endif // OPERATE_H