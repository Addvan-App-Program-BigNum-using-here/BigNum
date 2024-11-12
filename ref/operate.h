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
 * @param option option for clear
 */
msg bi_add(OUT bigint **dst, IN bigint **a, IN bigint **b, IN byte option);

/**
 * @brief bigint structure sub operation
 * @param dst pointer to result of bigint sub
 * @param a bigint structure a
 * @param b bigint structure b
 */
msg bi_sub(OUT bigint** dst, IN bigint** a, IN bigint** b, IN byte option);

// 주석 나중에
msg bi_mul(OUT bigint **dst, IN bigint **a, IN bigint **b);

#endif // OPERATE_H