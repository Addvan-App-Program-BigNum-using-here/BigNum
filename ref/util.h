#include "data_type.h"
#include "error_msg.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

/**
 * @brief New allocate memory for bigint struct
 * 
 * @param dst pointer to bigint struct
 * @param word_len length of bigint struct
 */
msg bi_new(bigint **dst, int word_len);

/**
 * @brief Delete allocated memory for bigint struct
 * 
 * @param dst pointer to bigint struct
 */
msg bi_delete(bigint **dst);

/**
 * @brief Refine bigint struct
 * 
 * @param src pointer to bigint struct
 */
msg bi_refine(bigint *src);

/**
 * @brief Processing_assign bigint struct
 * 
 * @param dst pointer to bigint struct
 * @param src source bigint struct
 */
msg bi_assign(bigint **dst, bigint *src);

/**
 * @brief Set bigint struct from array
 * 
 * @param dst pointer to bigint struct
 * @param sign sign of bigint struct
 * @param word_len length of bigint struct
 * @param a array of word
 * @param endian little endian == 0, big endian == 1
 */
msg bi_set_from_array(bigint **dst, int sign, int word_len, word *data, int endian);

/**
 * @brief Set bigint struct from string
 * 
 * @param dst pointer to bigint struct
 * @param int_str string of bigint
 * @param base base of string (2, 10, 16)
 */
msg bi_set_from_string(bigint **dst, char *int_str, int base);

/**
 * @brief Divide string to word custom to base 10
 * 
 * @param int_str string of bigint and return quotient
 * @param r return remainder
 * @param base base of string (2, 10, 16)
 */
msg String_Divide(char* int_str, word* a, int base);

msg bi_get_random(bigint **dst, int word_len);

/**
 * @brief Print bigint struct
 * 
 * @param dst pointer to bigint struct
 * @param base base of bigint struct (2, 10, 16)
 */
msg bi_print(bigint *dst, int base);