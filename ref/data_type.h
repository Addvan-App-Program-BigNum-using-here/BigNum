#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <stdint.h>

#define big_endian 0
#define little_endian 1
#define WORD_BITS 32

#define OUT
#define IN

typedef uint8_t byte; // Byte type
typedef uint32_t msg; // Error message control variation
typedef uint32_t word; // Word type
typedef uint64_t dword; // Double word type

/*************************************************
* Name:        bigint
*
* Description: big num information struct
*
* member variation:   - sign : positive or negative (positive == 0, negative == 1)
*                     - int word_len: length of bigint struct
*                     - word* a: array of word
**************************************************/
typedef struct{
    int sign;
    int word_len;
    word* a;
} bigint;


void int_to_char(IN word num, OUT char *str, IN int idx);
int char_to_int(IN char str);
uint64_t string_to_int(IN char* str, const IN int base);

#endif // DATA_TYPE_H