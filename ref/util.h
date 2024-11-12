
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
 */
msg bi_new(OUT bigint **dst, const IN int word_len);

/**
 * @brief Delete allocated memory for bigint struct
 *
 * @param dst pointer to bigint struct
 */
msg bi_delete(OUT bigint **dst);

/**
 * @brief Refine bigint struct
 *
 * @param src pointer to bigint struct
 */
msg bi_refine(OUT bigint *src);

/**
 * @brief Processing_assign bigint struct
 *
 * @param dst pointer to bigint struct
 * @param src source bigint struct
 */
msg bi_assign(OUT bigint **dst, IN bigint** src);

/**
 * @brief Expand bigint struct and fill data
 * @param dst pointer to bigint struct
 * @param word_len length of bigint struct
 * @param data array of word
 */
msg bi_expand(OUT bigint** dst, const IN int word_len, const IN word data);

/**
 * @brief Set bigint struct from array
 *
 * @param dst pointer to bigint struct
 * @param sign sign of bigint struct
 * @param word_len length of bigint struct
 * @param a array of word
 * @param endian little endian == 0, big endian == 1
 */
msg bi_set_from_array(OUT bigint **dst, const IN int sign, const IN int word_len, const IN word *data, const IN int endian);

/**
 * @brief Set bigint struct from string
 *
 * @param dst pointer to bigint struct
 * @param int_str string of bigint
 * @param base base of string (2, 10, 16)
 */
msg bi_set_from_string(OUT bigint **dst, IN char *int_str, const IN int base);

/**
 * @brief Divide string to word custom to base 10
 *
 * @param int_str string of bigint and return quotient
 * @param r return remainder
 * @param base base of string (2, 10, 16)
 */
msg String_Divide(OUT char* int_str, IN bigint **dst, IN int word_idx, const IN int base);

/**
 * @brief Print bigint struct
 *
 * @param dst pointer to bigint struct
 * @param base base of bigint struct (2, 10, 16)
 */
msg bi_print(IN bigint** dst, const IN int base);

/**
 * @brief Compare bigint struct
 *
 * @param a pointer to bigint struct
 * @param b pointer to bigint struct
 */
int bi_compare(IN bigint** a, IN bigint** b);

/**
 * @brief Compare absolute bigint struct
 *
 * @param a pointer to bigint struct
 * @param b pointer to bigint struct
 */
int bi_compare_abs(IN bigint** a, IN bigint** b);

// 주석 나중에
msg bi_shift_left(IN bigint** dst, const IN int shift_len);

#endif // UTIL_H
