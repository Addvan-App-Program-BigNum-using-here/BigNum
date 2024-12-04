#ifndef OPERATE_H
#define OPERATE_H

#include "data_type.h"
#include "msg_control.h"
#include "util.h"
#include "random.h"

#define mul_karachuba_ratio 4   // 카라츄바 곱셈 시 분할 기준
#define squ_karachuba_ratio 4   // 카라츄바 제곱 시 분할 기준
#define MAX_RECURSION_DEPTH 32  // 재귀 깊이
#define POOL_SIZE 8             // 카라츄바에 필요한 임시 변수 개수 (a_0, b_0, a_1, b_1, a_0b_0, a_1b_1, a_1_a_0, b_1_b_0)

// WORD_BITS에 따라 마스크와 시프트 연산을 처리하는 매크로 정의
#if WORD_BITS == 64
    #define WORD_MASK ((1ULL << 32) - 1)    // 16비트 마스크
    #define WORD_SHIFT 32                   // 상위 16비트 시프트
#elif WORD_BITS == 32
    #define WORD_MASK ((1U << 16) - 1)      // 16비트 마스크
    #define WORD_SHIFT 16                   // 상위 16비트 시프트
#elif WORD_BITS == 16
    #define WORD_MASK ((1U << 8) - 1)       // 8비트 마스크
    #define WORD_SHIFT 8                    // 상위 8비트 시프트
#elif WORD_BITS == 8
    #define WORD_MASK ((1U << 4) - 1)       // 4비트 마스크
    #define WORD_SHIFT 4                    // 상위 4비트 시프트
#else
    #error "Unsupported WORD_BITS value"
#endif

// 16비트 단위로 값을 추출하는 매크로
#define GET_LOWER_PART(word) ((word) & WORD_MASK)
#define GET_UPPER_PART(word) ((word) >> WORD_SHIFT)

// 메모리 풀 구조체
typedef struct {
    bigint* pool[MAX_RECURSION_DEPTH][POOL_SIZE];
    int current_depth;
    int max_word_size;  // 최대 워드 크기 저장
    bool initialized;
} karatsuba_mem_pool;

// 전역 메모리 풀 선언
__attribute__((used)) static karatsuba_mem_pool g_pool = {0};

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

/**
 * @brief bigint structure mul operation
 * @param dst pointer to result of bigint mul
 * @param a bigint structure a
 * @param b bigint structure b
 * @return msg
 */
msg bi_mul(OUT bigint **dst, IN bigint **a, IN bigint **b);

/**
 * @brief word mul operation
 * @param dst pointer to result of bigint mul
 * @param a word a
 * @param b word b
 * @return msg
 */
msg bi_mul_word(OUT bigint **dst, IN word a, IN word b);

/**
 * @brief bigint structure mul operation using Karachuba algorithm
 * @param dst pointer to result of bigint mul
 * @param a bigint structure a
 * @param b bigint structure b
 * @return msg
 */
msg bi_mul_karachuba(OUT bigint **dst, IN bigint **a, IN bigint **b, IN int karachuba_flag);

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
 * @param option option for WORD OR BINARY DIVISION
 * @return msg
 */
msg bi_div(OUT bigint **q, OUT bigint **r, IN bigint **a, IN bigint **b, IN int option);

/**
 * @brief bigint structure division operation using binary division
 * @param q pointer to quotient of bigint division
 * @param r pointer to remainder of bigint division
 * @param a dividend bigint operand
 * @param b divisor bigint operand
 * @return msg
 */
msg divc(OUT bigint** q, OUT bigint** r, IN bigint** a, IN bigint** b);

/**
 * @brief bigint structure division operation using word division
 * @param q pointer to quotient of bigint division
 * @param r pointer to remainder of bigint division
 * @param a dividend bigint operand
 * @param b divisor bigint operand
 * @param k b_m-1 * 2^k <= a < b_m * 2^k
 * @return msg
 */
msg divc_gener(OUT bigint** q, OUT bigint** r, IN bigint** a, IN bigint** b, IN int k);

/**
 * @brief bigint structure division sub operation
 * @param q pointer to quotient of word
 * @param r pointer to remainder of bigint division
 * @param a dividend bigint operand
 * @param b divisor bigint operand
 * @return msg
 */
msg divcc(OUT word* q, OUT bigint** r, IN bigint** a, IN bigint** b);

/**
 * @brief two word long division operation
 * @param q pointer to quotient of word
 * @param r pointer to remainder of bigint division
 * @param a dividend bigint operand
 * @param b divisor bigint operand
 * @return msg
 */
msg two_word_long_div(OUT word* q, IN bigint** a, IN word b);

/**
 * @brief bigint structure squaring operation
 * @param dst pointer to square of bigint
 * @param a dividend bigint to be squared
 * @return msg
 */
msg bi_squ(OUT bigint** dst, IN bigint** a);

/**
 * @brief bigint structure squaring operation by using Karachuba algorithm
 * @param dst pointer to square of bigint
 * @param a dividend bigint to be squared
 * @return msg
 */
msg bi_squ_karachuba(OUT bigint** dst, IN bigint** a, IN int karachuba_flag);

/**
 * @brief bigint structure squaring operation version to Multiplication and Squaring
 * @param dst pointer to square of bigint result
 * @param src pointer to squred of bigint
 * @param x pointer to square of bigint
 * @param n pointer to modulo of bigint
 * @return msg
 */
msg bi_exp_MS(OUT bigint** dst, IN bigint** src, IN bigint** x, IN bigint** n);

/**
 * @brief bigint structure squaring operation version to Right to Left
 * @param dst pointer to square of bigint result
 * @param src pointer to squred of bigint
 * @param x pointer to square of bigint
 * @param n pointer to modulo of bigint
 * @return msg
 */
msg bi_exp_R_TO_L(OUT bigint** dst, IN bigint** src, IN bigint** x, IN bigint** n);

/**
 * @brief bigint structure squaring operation version to Left to Right
 * @param dst pointer to square of bigint result
 * @param src pointer to squred of bigint
 * @param x pointer to square of bigint
 * @param n pointer to modulo of bigint
 * @return msg
 */
msg bi_exp_L_TO_R(OUT bigint** dst, IN bigint** src, IN bigint** x, IN bigint** n);

/**
 * @brief bigint structure barret reduction
 * @param dst pointer to reduction of bigint result
 * @param a pointer to reduced of bigint
 * @param n pointer to modular of bigint
 * @return msg
 */
msg barret_reduction(OUT IN bigint** dst, IN bigint** a, IN bigint** n, IN bigint** n_barret);

/**
 * @brief Initialize barret reduction
 * @param barret_t pointer to pre calculate bigint using barret reduction
 * @param barret_n pointer to modular of bigint
 * @param barret_word_len pointer to word length of bigint
 * @return msg
 */
msg init_barret_N(OUT bigint** barret_t, IN bigint** barret_n, IN int barret_word_len);
#endif // OPERATE_H