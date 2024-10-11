#include "random.h"

/*************************************************
 * Name:        bi_get_random
 *
 * Description: New allocate bigint struct and fill it with random values
 *
 * Arguments:   - bigint** dst: pointer to bigint struct
 *              - int word_len: length of bigint struct
 **************************************************/
msg bi_get_random(bigint **dst, int word_len)
{
    msg result_msg = bi_new(dst, word_len);
    if (result_msg == BI_ALLOC_FAIL)
    {
        return RANDOM_FAIL;
    }

    (*dst)->sign = rand() % 2;
    for (int i = 0; i < word_len; i++)
    {
        (*dst)->a[i] = rand() & 0xFFFFFFFF;
    }
    return RANDOM_SUCCESS;
}