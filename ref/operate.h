#ifndef OPERATE_H
#define OPERATE_H

#include "data_type.h"
#include "msg_control.h"
#include "util.h"

#define flag 10 // karachuba에서 base case 수행 시 일반 곱셈의 기준

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
 * @brief bigint structure division operation
 * @param q pointer to quotient of bigint division
 * @param r pointer to remainder of bigint division
 * @param a dividend bigint operand
 * @param b divisor bigint operand
 * @return msg
 */

msg bi_div(OUT bigint **q, OUT bigint **r, IN bigint **a, IN bigint **b);

#endif // OPERATE_H