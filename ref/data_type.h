#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <stdint.h>
#include <stdio.h>

#define big_endian 0              ///< big endian
#define little_endian 1           ///< little endian
#define WORD_BITS 32              ///< Word bits
#define APPEND 0                  ///< File append
#define CLEAR 1                   ///< File clear and write

#define OUT
#define IN
#define seperator "-----------------------------------------"

typedef uint8_t byte;             ///< byte type
typedef uint16_t hword;           ///< Half word type
typedef uint32_t msg;             ///< Error message control variation
typedef uint32_t word;            ///< Word type
typedef uint64_t dword;           ///< Double word type

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
typedef struct {
    byte sign;       ///< positive or negative (positive == 0, negative == 1)
    int word_len;   ///< length of bigint struct
    word* a;        ///< array of word
} bigint;

/**
 * @brief Transfer integer to string
 *
 * @param num integer
 * @param str string
 * @param idx index of string
 */
void int_to_char(IN word num, OUT char *str, IN int idx);

/**
 * @brief Transfer char to integer
 *
 * @param str string
 * @return integer
 */
int char_to_int(IN char str);

/**
 * @brief Convert string to integer
 *
 * @param str string of bigint
 * @param base integer base
 * @return result of integer
 */
uint64_t string_to_int(IN char* str, const IN int base);

// 여기 주석 추가하기
int bigint_to_hex(IN bigint** src, OUT char* str);

// 여기 주석 추가하기
word byte_to_int(IN byte* input, const IN int byte_len);

// 여기 주석 추가하기
msg operate_string_cat(OUT char* result, IN bigint** a, IN bigint** b, IN bigint** c, IN char operate);

// 여기 주석 추가하기
//char* String_refine(const IN char* str);

#endif // DATA_TYPE_H