#ifndef DATA_TYPE_H
#define DATA_TYPE_H


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define WORD_BITS 32    ///< Word bits
#define APPEND 0        ///< File append
#define CLEAR 1         ///< File clear and write
#define _POSIX_C_SOURCE 200809L

#define OUT
#define IN
#define seperator "-----------------------------------------"
#define FROM_init "[BI SET FROM]"
#define SHIFT_init "[Shift]"
#define GET_LOWER_init "[Get Lower]"
#define CAT_init "[Cat]"

typedef uint8_t byte;   ///< byte type
typedef uint16_t hword; ///< Half word type
typedef uint32_t msg;   ///< Error message control variation
typedef uint32_t word;  ///< Word type
typedef uint64_t dword; ///< Double word type

#define min(a, b) ((a) < (b) ? (a) : (b)) ///< min value
#define max(a, b) ((a) > (b) ? (a) : (b)) ///< max value

/**
 * @struct bigint
 * @brief big num information struct
 *
 * @var bigint::sign
 * positive or negative (positive == 0, negative == 1)
 *
 * @var bigint::word_len
 * length of bigint struct
 *
 * @var bigint::a
 * array of word
 */
typedef struct{
    byte sign;    ///< positive or negative (positive == 0, negative == 1)
    int word_len; ///< length of bigint struct
    word *a;      ///< array of word
} bigint;

/**
 * @brief Transfer char to integer
 *
 * @param str string
 * @return integer
 */
int char_to_int(IN char str);

/**
 * @brief Convert bigint to hex string
 *
 * @param src bigint structure
 * @param str return string value
 * @return result of integer, FAIL == -1
 */
int bigint_to_hex(OUT char* str, IN bigint** src);

/**
 * @brief Convert byte(array) to integer
 *
 * @param input byte array
 * @param byte_len length of byte array
 * @return word of integer
 */
word byte_to_uint(IN byte *input, const IN int byte_len);

/**
* @brief Convert integer to string
*
* @param str char array
* @param num integer
*/
char* int_to_string(IN int num);

#endif // DATA_TYPE_H