#include "operate.h"

/*************************************************
* Name:        bi_add
*
* Description: Bigint adddition
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - bigint** a: bigint struct
*              - bigint** b: bigint struct
**************************************************/
msg bi_add(OUT bigint** dst, IN bigint** a, IN bigint** b){
    printf("aaa");
    bigint** tmp = NULL;
    byte carry = 0;
    msg result_msg;
    int max_word_len = 0, sign = 0;

    if(a == NULL || b == NULL)    return BI_ADD_FAIL;
    if(dst != NULL)    bi_delete(dst);

//    if(a->sign == 0 && b->sign == 1)    return bi_sub(dst, a, b); // a - b
//    if(a->sign == 1 && b->sign == 0)    return bi_sub(dst, b, a); // b - a
//    if(a->sign == 1 && b->sign == 1)    sign = 1; // -(a + b)


    // 상수 시간 연산을 위해 a와 b의 크기를 맞춰서 연산 수행
    if ((*a)->word_len >= (*b)->word_len){
        max_word_len = (*a)->word_len;
        result_msg = bi_assign(tmp, b);
    } else{
        max_word_len = (*b)->word_len;
        result_msg = bi_assign(tmp, a);
    }

    if(result_msg != BI_SET_ASSIGN_SUCCESS)    return BI_ADD_FAIL;
    if(bi_expand(tmp, max_word_len, 0) != BI_EXPAND_SUCCESS)    return BI_ADD_FAIL;

    if(bi_new(dst, max_word_len) != BI_ALLOC_SUCCESS)    return BI_ADD_FAIL;

    // 덧셈 연산 수행
    for(int i = 0; i < max_word_len; i++){
        carry = ((*a)->a[i] > 0xffffffff - ((*b)->a[i] + carry)) ? 1 : 0; // carry bit 계산
        (*dst)->a[i] = (word)((*a)->a[i] + (*b)->a[i]);
    }

    bi_refine(*dst);

    return BI_ADD_SUCCESS;
}