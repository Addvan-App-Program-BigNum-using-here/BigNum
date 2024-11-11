#ifndef OPERATE_H
#define OPERATE_H

#include "data_type.h"
#include "msg_control.h"
#include "util.h"

/**
 * @brief bigint structure add operation
 * @param dst pointer to result of bigint add
 * @param a bigint structure a
 * @param b bigint structure b
 */
msg bi_add(OUT bigint **dst, IN bigint **a, IN bigint **b);

/**
 * @brief bigint structure sub operation
 * @param dst pointer to result of bigint sub
 * @param a bigint structure a
 * @param b bigint structure b
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

#endif // OPERATE_H