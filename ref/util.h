
#ifndef UTIL_H
#define UTIL_H

#include "data_type.h"
#include "msg_control.h"
#include "operate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <sys/time.h>
// 윈도우일 때는 time.h 인 것으로 판단
#include <time.h>
#include <stdarg.h>

#define CHECK_FUNCTION_RUN_ONE_TIME(func, dst, result_msg, param_types, ...) \
    check_function_run_one_time(func, dst, result_msg, param_types, \
        (sizeof((void*[]){__VA_ARGS__}) / sizeof(void*)), \
        __VA_ARGS__)

#define MAX_PARAMS 5
#define COMPOSITE 0
#define PRIME 1
typedef enum {
    TYPE_BIGINT_PTR,     // bigint**
    TYPE_INT_PTR,       // int*
    TYPE_WORD_ARR_PTR,  // word array*
    TYPE_STR_PTR        // char*
} ParamType;
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
msg bi_set_from_array(OUT bigint **dst, const IN int word_len, const IN word *data, const IN int endian);

/**
 * @brief Set bigint struct from string
 *
 * @param dst pointer to bigint struct
 * @param int_str string of bigint
 * @param base base of string (2, 10, 16)
 * @return message SUCCESS or FAIL
 */
msg bi_set_from_string(OUT bigint **dst, IN char *int_str, IN int base);

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
 * @param func test function pointer
 * @param dst pointer to bigint struct result
 * @param result_msg pointer to result_msg
 * @param param_types parameter type
 * @param param_count parameter count
 * @return runtime
 */
double check_function_run_one_time(void* func, bigint** dst, msg* result_msg, ParamType* param_types, int param_count, ...);

/**
 * @brief get function power decomposition
 *
 * @param n word number
 * @param powers power decomposition array
 * @return array size
 */
int get_power_decomposition(word n, int* powers);

/**
 * @brief bigint structure Miller-Rabin primality test
 * @param dst pointer to square of bigint
 * @param a dividend bigint to be squared
 * @return msg
 */
msg miller_rabin_primality(OUT bigint** temp, IN bigint **n, IN int k);

/**
 * @brief bigint structure gcd operation
 * @param dst pointer to square of bigint
 * @param a dividend bigint to be squared
 * @return msg
 */
msg bi_gcd(OUT bigint** dst, IN bigint** a, IN bigint** b);

/**
 * @brief bigint structure Extended Euclidean Algorithm
 * @param dst pointer to square of bigint
 * @param a dividend bigint to be squared
 * @return msg
 */
msg bi_EEA(OUT bigint** gcd, OUT bigint** x, OUT bigint** y, IN bigint** a, IN bigint** b);
#endif // UTIL_H
