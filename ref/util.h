
#ifndef UTIL_H
#define UTIL_H

#include "data_type.h"
#include "msg_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/**
 * @brief New allocate memory for bigint struct
 *
 * @param dst pointer to bigint struct
 * @param word_len length of bigint struct
 * @return message SUCCESS or FAIL
 */
msg bi_new(OUT bigint **dst, const IN int word_len);

/**
 * @brief Delete allocated memory for bigint struct
 *
 * @param dst pointer to bigint struct
 * @return message SUCCESS or FAIL
 */
msg bi_delete(OUT bigint **dst);

/**
 * @brief Refine bigint struct
 *
 * @param src pointer to bigint struct
 * @return message SUCCESS or FAIL
 */
msg bi_refine(OUT bigint** src);

/**
 * @brief Processing_assign bigint struct
 *
 * @param dst pointer to bigint struct
 * @param src source bigint struct
 * @return message SUCCESS or FAIL
 */
msg bi_assign(OUT bigint **dst, IN bigint **src);

/**
 * @brief resize bigint struct
 *
 * @param dst pointer to bigint struct
 * @param word_len length of bigint struct
 * @return message SUCCESS or FAIL
 */
msg bi_resize(OUT bigint** dst, const IN int word_len);

/**
 * @brief Expand bigint struct and fill data
 * @param dst pointer to bigint struct
 * @param word_len length of bigint struct
 * @param data array of word
 * @return message SUCCESS or FAIL
 */
msg bi_expand(OUT bigint **dst, const IN int word_len, const IN word data);

/**
 * @brief Set bigint struct from array
 *
 * @param dst pointer to bigint struct
 * @param sign sign of bigint struct
 * @param word_len length of bigint struct
 * @param a array of word
 * @param endian little endian == 0, big endian == 1
 * @return message SUCCESS or FAIL
 */
msg bi_set_from_array(OUT bigint **dst, const IN int sign, const IN int word_len, const IN word *data, const IN int endian);

/**
 * @brief Set bigint struct from string
 *
 * @param dst pointer to bigint struct
 * @param int_str string of bigint
 * @param base base of string (2, 10, 16)
 * @return message SUCCESS or FAIL
 */
msg bi_set_from_string(OUT bigint **dst, IN char *int_str, const IN int base);

/**
 * @brief Print bigint struct
 *
 * @param dst pointer to bigint struct
 * @param base base of bigint struct (2, 10, 16)
 * @return message SUCCESS or FAIL
 */
msg bi_print(IN bigint **dst, const IN int base);

/**
 * @brief Compare bigint struct
 *
 * @param a pointer to bigint struct
 * @param b pointer to bigint struct
 * @return result of compare (a > b => 1, a < b => -1, a == b => 0)
 */
int bi_compare(IN bigint **a, IN bigint **b);

/**
 * @brief Compare absolute bigint struct
 *
 * @param a pointer to bigint struct
 * @param b pointer to bigint struct
 * @return result of compare (a > b => 1, a < b => -1, a == b => 0)
 */
int bi_compare_abs(IN bigint **a, IN bigint **b);

/**
 * @brief bigint structure shift left operator
 *
 * @param dst pointer to bigint struct result
 * @param src pointer to bigint struct source
 * @param shift_len length of shift
 * @return message SUCCESS or FAIL
 */
msg bi_shift_left(OUT bigint **dst, IN bigint **src, const IN int shift_len);

/**
 * @brief bigint structure shift right operator
 *
 * @param dst pointer to bigint struct result
 * @param src pointer to bigint struct source
 * @param shift_len length of shift
 * @return message SUCCESS or FAIL
 */
msg bi_shift_right(OUT bigint **dst, IN bigint **src, const IN int shift_len);

/**
 * @brief bigint structure modulation operator
 *
 * @param dst pointer to bigint struct result
 * @param src pointer to bigint struct source
 * @param mod_len length of shift
 * @return message SUCCESS or FAIL
 */
msg bi_mod(OUT bigint **dst, IN bigint **src, IN int mod_len);

/**
 * @brief bigint structure concatenate operator => a||b
 *
 * @param dst pointer to bigint struct result
 * @param a pointer to bigint struct concate
 * @param b pointer to bigint struct concated
 * @return message SUCCESS or FAIL
 */
msg bi_cat(OUT bigint **dst, IN bigint **a, IN bigint **b);

bool bi_is_zero(const bigint *num);
#endif // UTIL_H
