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

    bigint* tmp = NULL;
    byte carry = 0;
    msg result_msg;
    int max_word_len = 0, sign = 0;

    if(a == NULL || b == NULL)    return MEM_NOT_ALLOC;
    if(dst != NULL)    bi_delete(dst);

//    if(a->sign == 0 && b->sign == 1)    return bi_sub(dst, a, b); // a - b
//    if(a->sign == 1 && b->sign == 0)    return bi_sub(dst, b, a); // b - a
//    if(a->sign == 1 && b->sign == 1)    sign = 1; // -(a + b)


    // 상수 시간 연산을 위해 a와 b의 크기를 맞춰서 연산 수행
    if ((*a)->word_len >= (*b)->word_len){
        max_word_len = (*a)->word_len;
        result_msg = bi_assign(&tmp, b);
    } else{
        max_word_len = (*b)->word_len;
        result_msg = bi_assign(&tmp, a);
    }

    if(result_msg != BI_SET_ASSIGN_SUCCESS)    return BI_SET_ASSIGN_FAIL;
    if(bi_expand(&tmp, max_word_len, 0) != BI_EXPAND_SUCCESS)    return BI_EXPAND_FAIL;

    if(bi_new(dst, max_word_len + 1) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;

    // 덧셈 연산 수행
    for(int i = 0; i < max_word_len; i++){
        (*dst)->a[i] = (word)((*a)->a[i] + (*b)->a[i] + carry);
        carry = ((*a)->a[i] > 0xffffffff - ((*b)->a[i] + carry) || (*b)->a[i] > 0xffffffff - ((*a)->a[i] + carry) || ((*a)->a[i] == 0xffffffff && (*b)->b[i] == 0xffffffff)) ? 1 : 0; // carry bit 계산
    }
    (*dst)->a[max_word_len] = carry;

    bi_delete(&tmp);
    bi_refine(*dst);

    return BI_ADD_SUCCESS;
}

/*************************************************
* Name:        bi_sub
*
* Description: Bigint substraction
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - bigint** a: bigint struct
*              - bigint** b: bigint struct
**************************************************/
msg bi_sub(OUT bigint** dst, IN bigint** a, IN bigint** b){

    bigint* tmp = NULL;
    byte barrow = 0;
    int sign = 0;

    if(a == NULL || b == NULL)    return MEM_NOT_ALLOC;
    if(dst != NULL)    bi_delete(dst);

    if((*a)->sign == 0 && (*b)->sign == 1){ // a - (-b) = a + b
        if(bi_add(dst, a, b) != BI_ADD_SUCCESS)    return BI_SUB_FAIL;
        return BI_SUB_SUCCESS;
    }else if((*a)->sign == 1 && (*b)->sign == 0){ // -a - b = -(a + b)
        if(bi_add(dst, a, b) != BI_ADD_SUCCESS)    return BI_SUB_FAIL;
        (*dst)->sign = 1;
        return BI_SUB_SUCCESS;
    }else if((*a)->sign == 1 && (*b)->sign == 1){ // -a - (-b) = b - a
        if(bi_sub(dst, b, a) != BI_SUB_SUCCESS)    return BI_SUB_FAIL;
        return BI_SUB_SUCCESS;
    }

    // b가 a보다 더 큰 경우
    printf("%d\n", bi_compare_abs(a, b));
    if(bi_compare_abs(a, b) == -1){
        // b - a
        if(bi_sub(dst, b, a) != BI_SUB_SUCCESS)    return BI_SUB_FAIL;
        (*dst)->sign = 1; // -(b - a)
        return BI_SUB_SUCCESS;
    }

    // 상수 시간 연산을 위해 a와 b의 크기를 맞춰서 연산 수행 -> 무조건 a >= b
    if(bi_assign(&tmp, b) != BI_SET_ASSIGN_SUCCESS)    return BI_SET_ASSIGN_FAIL; // b를 tmp에 복사
    if(bi_expand(&tmp, (*a)->word_len, 0) != BI_EXPAND_SUCCESS)    return BI_EXPAND_FAIL; // tmp의 길이를 a와 같게 만듦
    if(bi_new(dst, (*a)->word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL; // 결과값 할당

    // 뺄셈 연산 수행
    for(int i = 0; i < (*a)->word_len; i++){
        (*dst)->a[i] = (word)((*a)->a[i] + barrow * 0xFFFFFFFF - (*b)->a[i]);
        barrow = ((*a)->a[i] < (*b)->a[i] + barrow) ? 1 : 0; // carry bit 계산
    }

    bi_delete(&tmp);
    bi_refine(*dst);
    return BI_SUB_SUCCESS;
}