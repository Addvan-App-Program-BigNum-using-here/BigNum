#include <stdint.h>

#define big_endian 0
#define little_endian 1

typedef uint32_t msg; // Error message control variation
typedef uint32_t word; // Word type


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