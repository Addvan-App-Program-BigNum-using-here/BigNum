
#ifndef UTIL_H
#define UTIL_H

#include "data_type.h"
#include "msg_control.h"
#include "operate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
// 윈도우일 때는 time.h 인 것으로 판단
#include <time.h>

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
 * @brief bigint structure get lower bigint operator
 *
 * @param dst pointer to bigint struct result
 * @param src pointer to bigint struct source
 * @param mod_len length of shift
 * @return message SUCCESS or FAIL
 */
msg bi_get_lower(OUT bigint **dst, IN bigint **src, IN int mod_len);

/**
 * @brief bigint structure concatenate operator => a||b
 *
 * @param dst pointer to bigint struct result
 * @param a pointer to bigint struct concate
 * @param b pointer to bigint struct concated
 * @return message SUCCESS or FAIL
 */
msg bi_cat(OUT bigint **dst, IN bigint **a, IN bigint **b);

/**
 * @brief check bigint is zero
 *
 * @param num bigint structure to check zero
 * @return message TRUE or FALSE
 */
msg bi_is_zero(bigint **num);

/**
 * @brief get function runtime
 *
 * @param msg (*func) test function pointer
 * @param dst pointer to bigint struct result
 * @param a pointer to bigint struct of input1
 * @param b pointer to bigint struct of input2
 * @return runtime
 */
double check_function_run_one_time_two_parm_bigint(IN msg (*func)(OUT bigint**, IN bigint**), OUT bigint** dst, IN bigint** a, IN msg* result_msg);
double check_function_run_one_time_two_parm_int(IN msg (*func)(OUT bigint**, IN int), OUT bigint** dst, IN int n, IN msg* result_msg);
double check_function_run_one_time_three_parm_bigint(IN msg (*func)(OUT bigint**, IN bigint**, IN bigint**), OUT bigint** dst, IN bigint** a, IN bigint** b, IN msg* result_msg);
double check_function_run_one_time_three_parm_int(IN msg (*func)(OUT bigint**, IN bigint**, IN int), OUT bigint** dst, IN bigint** a, IN int n, IN msg* result_msg);
double check_function_run_one_time_four_parm_bigint(IN msg (*func)(OUT bigint**, IN bigint**, IN bigint**, IN bigint**), OUT bigint** dst, IN bigint** a, IN bigint** b, IN bigint** c, IN msg* result_msg);
#endif // UTIL_H
