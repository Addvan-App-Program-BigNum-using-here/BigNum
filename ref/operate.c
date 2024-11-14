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
msg bi_add(OUT bigint **dst, IN bigint **a, IN bigint **b)
{
    byte carry = 0;
    msg result_msg;
    int max_word_len = 0;

    if (a == NULL || b == NULL)
        return MEM_NOT_ALLOC;
    if (dst != NULL)
    {
        if (*dst == *a || *dst == *b)
        { // dst가 a나 b인 경우
            bigint *temp = NULL;
            if (bi_add(&temp, a, b) != BI_ADD_SUCCESS)
                return BI_ADD_FAIL;
            if (bi_assign(dst, &temp) != BI_SET_ASSIGN_SUCCESS)
                return BI_ADD_FAIL;
            if (bi_delete(&temp) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
            return BI_ADD_SUCCESS;
        }
        else
        { // dst가 a와 b가 아닌 경우
            if (bi_delete(dst) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
        }
    }

    if ((*a)->sign == 0 && (*b)->sign == 1)
    { // a - b
        (*b)->sign = 0;
        if (bi_sub(dst, a, b) != BI_SUB_SUCCESS)
            return BI_ADD_FAIL; // a - b
        return BI_ADD_SUCCESS;
    }
    if ((*a)->sign == 1 && (*b)->sign == 0)
    { // -a + b = b - a
        (*a)->sign = 0;
        if (bi_sub(dst, b, a) != BI_SUB_SUCCESS)
            return BI_SUB_FAIL; // b - a
        return BI_ADD_SUCCESS;
    }
    if ((*a)->sign == 1 && (*b)->sign == 1)
    { // -a + -b = -(a + b)
        (*a)->sign = 0;
        (*b)->sign = 0;
        if (bi_add(dst, a, b) != BI_ADD_SUCCESS)
            return BI_ADD_FAIL; // a + b
        (*dst)->sign = 1;       // -(a + b)
        return BI_ADD_SUCCESS;
    }

    // 상수 시간 연산을 위해 a와 b의 크기를 맞춰서 연산 수행
    if ((*a)->word_len >= (*b)->word_len)
    {
        max_word_len = (*a)->word_len;
        result_msg = bi_expand(b, max_word_len, 0); // 길이 맞추기
        if (result_msg != BI_EXPAND_SUCCESS)
            return BI_EXPAND_FAIL;
    }
    else
    {
        max_word_len = (*b)->word_len;
        result_msg = bi_expand(a, max_word_len, 0); // 길이 맞추기
        if (result_msg != BI_EXPAND_SUCCESS)
            return BI_EXPAND_FAIL;
    }

    if (bi_new(dst, max_word_len + 1) != BI_ALLOC_SUCCESS)
    {
        if (bi_refine(*a) != BI_SET_REFINE_SUCCESS)
            return BI_SET_REFINE_FAIL;
        if (bi_refine(*b) != BI_SET_REFINE_SUCCESS)
            return BI_SET_REFINE_FAIL;
        return BI_ALLOC_FAIL;
    }

    // 덧셈 연산 수행
    for (int i = 0; i < max_word_len; i++)
    {
        (*dst)->a[i] = (word)((*a)->a[i] + (*b)->a[i] + carry);
        carry = ((*a)->a[i] > 0xffffffff - ((*b)->a[i] + carry) || (*b)->a[i] > 0xffffffff - ((*a)->a[i] + carry) || ((*a)->a[i] == 0xffffffff && (*b)->a[i] == 0xffffffff)) ? 1 : 0; // carry bit 계산
    }
    if (carry)
        (*dst)->a[max_word_len] = carry;

    if (bi_refine(*dst) != BI_SET_REFINE_SUCCESS)
    {
        if (bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
        return BI_SET_REFINE_FAIL;
    }
    if (bi_refine(*a) != BI_SET_REFINE_SUCCESS)
    {
        if (bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
        return BI_SET_REFINE_FAIL;
    }
    if (bi_refine(*b) != BI_SET_REFINE_SUCCESS)
    {
        if (bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
        return BI_SET_REFINE_FAIL;
    }
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
msg bi_sub(OUT bigint **dst, IN bigint **a, IN bigint **b)
{
    byte barrow = bi_compare_abs(a, b) >= 0 ? 0 : 1; // 여기 첫 barrow 수행할 때 a >= b일 때 0, a < b 일 때 1

    if (a == NULL || b == NULL)
        return MEM_NOT_ALLOC;
    if (dst != NULL)
    {
        if (*dst == *a || *dst == *b)
        { // dst가 a나 b인 경우
            bigint *temp = NULL;
            if (bi_sub(&temp, a, b) != BI_SUB_SUCCESS)
                return BI_SUB_FAIL;
            if (bi_assign(dst, &temp) != BI_SET_ASSIGN_SUCCESS)
                return BI_SUB_FAIL;
            if (bi_delete(&temp) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
            return BI_SUB_SUCCESS;
        }
        else
        { // dst가 a와 b가 아닌 경우
            if (bi_delete(dst) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
        }
    }

    if ((*a)->sign == 0 && (*b)->sign == 1)
    { // a - (-b) = a + b
        (*b)->sign = 0;
        if (bi_add(dst, a, b) != BI_ADD_SUCCESS)
            return BI_SUB_FAIL;
        return BI_SUB_SUCCESS;
    }
    else if ((*a)->sign == 1 && (*b)->sign == 0)
    { // -a - b = -(a + b)
        (*a)->sign = 0;
        if (bi_add(dst, a, b) != BI_ADD_SUCCESS)
            return BI_SUB_FAIL;
        (*dst)->sign = 1;
        return BI_SUB_SUCCESS;
    }
    else if ((*a)->sign == 1 && (*b)->sign == 1)
    { // -a - (-b) = b - a
        (*a)->sign = 0;
        (*b)->sign = 0;
        if (bi_sub(dst, b, a) != BI_SUB_SUCCESS)
            return BI_SUB_FAIL;
        return BI_SUB_SUCCESS;
    }

    // b가 a보다 더 큰 경우
    if (bi_compare_abs(a, b) == -1)
    {
        // b - a
        if (bi_sub(dst, b, a) != BI_SUB_SUCCESS)
            return BI_SUB_FAIL;
        (*dst)->sign = 1; // -(b - a) // 모듈러 체크
        return BI_SUB_SUCCESS;
    }

    // 상수 시간 연산을 위해 a와 b의 크기를 맞춰서 연산 수행 -> 무조건 a >= b
    if (bi_expand(b, (*a)->word_len, 0) != BI_EXPAND_SUCCESS)
    { // tmp의 길이를 a와 같게 만듦
        return BI_EXPAND_FAIL;
    }

    if (bi_new(dst, (*a)->word_len) != BI_ALLOC_SUCCESS)
        return BI_ALLOC_FAIL;

    // 뺄셈 연산 수행
    for (int i = 0; i < (*a)->word_len; i++)
    {
        (*dst)->a[i] = (word)(barrow * 0xFFFFFFFF - (*b)->a[i] + (*a)->a[i]);
        barrow = (((*a)->a[i] < (*b)->a[i] + barrow) || ((*b)->a[i] == 0xffffffff && barrow)) ? 1 : 0; // barrow bit 계산
    }

    if (bi_refine(*b) != BI_SET_REFINE_SUCCESS)
    {
        if (bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
        return BI_SET_REFINE_FAIL;
    }
    if (bi_refine(*dst) != BI_SET_REFINE_SUCCESS)
    {
        if (bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
        return BI_SET_REFINE_FAIL;
    }
    return BI_SUB_SUCCESS;
}

/*************************************************
 * Name:        bi_mul
 *
 * Description: Bigint multiplication
 *
 * Arguments:   - bigint** dst: pointer to bigint struct
 *              - bigint** a: bigint struct
 *              - bigint** b: bigint struct
 **************************************************/
msg bi_mul(OUT bigint **dst, IN bigint **a, IN bigint **b)
{
    // 데이터 임시 변수들 선언
    bigint *temp1 = NULL;
    bigint *temp2 = NULL;
    bigint *temp3 = NULL;
    // 여기 temp 줄일 수 있는 방법 생각해보자.
    msg result_msg;
    word b_i;
    int temp_len = 0, idx_temp1, idx_temp2;
    byte a_sign = 0, b_sign = 0;

    // 에러 케이스 정의
    if (a == NULL || b == NULL)
        return MEM_NOT_ALLOC;
    if (dst != NULL)
    {
        if (*dst == *a || *dst == *b)
        { // dst가 a나 b인 경우
            bigint *temp = NULL;
            if (bi_mul(&temp, a, b) != BI_MUL_SUCCESS)
                return BI_MUL_FAIL;
            if (bi_assign(dst, &temp) != BI_SET_ASSIGN_SUCCESS)
                return BI_MUL_FAIL;
            if (bi_delete(&temp) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
            return BI_MUL_SUCCESS;
        }
        else
        { // dst가 a와 b가 아닌 경우
            if (bi_delete(dst) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
        }
    }

    if (bi_compare_abs(a, b) == -1)
    { // a < b
        result_msg = bi_mul(dst, b, a);
        if (result_msg != BI_MUL_SUCCESS)
            return BI_MUL_FAIL;
        return BI_MUL_SUCCESS;
    }

    // 최대 길이 : n + m
    int max_len = (*a)->word_len + (*b)->word_len;
    if (bi_new(dst, max_len) != BI_ALLOC_SUCCESS)
        return BI_ALLOC_FAIL;

    temp_len = ((*a)->word_len * 2) / 2; // 박스 개수

    a_sign = (*a)->sign;
    b_sign = (*b)->sign;
    (*a)->sign = 0;
    (*b)->sign = 0;

    // 반복문 밖에가 작은 값, 안쪽이 큰 값
    for (int i = ((*b)->word_len * 2) - 1; i >= 0; i--)
    { // word_len == 2
        result_msg = bi_new(&(temp1), temp_len);
        if (result_msg != BI_ALLOC_SUCCESS)
            goto MUL_EXIT;
        result_msg = bi_new(&(temp2), temp_len);
        if (result_msg != BI_ALLOC_SUCCESS)
            goto MUL_EXIT;

        b_i = (i % 2 == 0) ? (*b)->a[i / 2] & 0xFFFF : (*b)->a[i / 2] >> 16; // 16비트 단위로 나누어 곱셈 수행
        idx_temp1 = 0;
        idx_temp2 = 0;
        for (int j = 0; j < (*a)->word_len * 2; j++)
        { // word_len == 2
            if (j % 2 == 0)
                (*temp1).a[idx_temp1++] = (word)(b_i * ((*a)->a[j / 2] & 0xFFFF)); // 하위 16비트 곱
            else
                (*temp2).a[idx_temp2++] = (word)(b_i * ((*a)->a[j / 2] >> 16)); // 상위 16비트 곱
        }

        result_msg = bi_shift_left(&temp1, &temp1, i * 16); // 16비트 단위로 밀기
        if (result_msg != BI_SHIFT_SUCCESS)
            goto MUL_EXIT;
        result_msg = bi_shift_left(&temp2, &temp2, (i + 1) * 16); // 16비트 단위로 밀기
        if (result_msg != BI_SHIFT_SUCCESS)
            goto MUL_EXIT;

        result_msg = bi_add(&temp3, &temp1, &temp2); // 덧셈 수행
        if (result_msg != BI_ADD_SUCCESS)
            goto MUL_EXIT;

        result_msg = bi_add(dst, dst, &temp3); // 덧셈 수행
        if (result_msg != BI_ADD_SUCCESS)
            goto MUL_EXIT;

        if (bi_delete(&temp3) != BI_FREE_SUCCESS)
            goto MUL_EXIT;
        if (bi_delete(&temp2) != BI_FREE_SUCCESS)
            goto MUL_EXIT;
        if (bi_delete(&temp1) != BI_FREE_SUCCESS)
            goto MUL_EXIT;
    }

    // 부호 처리
    (*dst)->sign = a_sign ^ b_sign; // XOR 연산으로 부호 처리 다르면 음수, 같으면 양수
    (*a)->sign = a_sign;
    (*b)->sign = b_sign;

    if (bi_refine(*dst) != BI_SET_REFINE_SUCCESS)
        goto MUL_EXIT;
    return BI_MUL_SUCCESS;

MUL_EXIT:
    if (bi_delete(&temp1) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&temp2) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&temp3) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    log_msg(result_msg);
    return BI_MUL_FAIL;
}

/*************************************************
 * Name:        bi_mul_karachuba
 *
 * Description: Bigint multiplication using Karachuba algorithm
 *
 * Arguments:   - bigint** dst: pointer to bigint struct
 *              - bigint** a: bigint struct
 *              - bigint** b: bigint struct
 **************************************************/
msg bi_mul_karachuba(OUT bigint **dst, IN bigint **a, IN bigint **b)
{
    int max_word_len = 0, half_word_len = 0, min_word_len = 0;
    //    char str[1024];
    bigint *a_0 = NULL;
    bigint *b_0 = NULL;
    bigint *a_1 = NULL;
    bigint *b_1 = NULL;
    bigint *a_0b_0 = NULL;
    bigint *a_1b_1 = NULL;
    bigint *a_1_a_0 = NULL;
    bigint *b_1_b_0 = NULL;
    msg result_msg = 0;
    byte a_sign = 0, b_sign = 0;

    if (a == NULL || b == NULL)
        return MEM_NOT_ALLOC;

    // dst가 NULL이 아닌 경우 메모리 해제
    if (dst != NULL)
    {
        if (*dst == *a || *dst == *b)
        { // dst가 a나 b인 경우
            bigint *temp = NULL;
            if (bi_mul_karachuba(&temp, a, b) != BI_MUL_SUCCESS)
                return BI_MUL_FAIL;
            if (bi_assign(dst, &temp) != BI_SET_ASSIGN_SUCCESS)
                return BI_MUL_FAIL;
            if (bi_delete(&temp) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
            return BI_MUL_SUCCESS;
        }
        else
        { // dst가 a와 b가 아닌 경우
            if (bi_delete(dst) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
        }
    }

    // base case에서 카라츄바가 아닌 일반 곱셈 수행을 위한 연산
    min_word_len = min((*a)->word_len, (*b)->word_len);
    if (flag > min_word_len)
    {
        result_msg = bi_mul(dst, a, b);
        if (result_msg != BI_MUL_SUCCESS)
            return BI_MUL_FAIL;
        return BI_MUL_SUCCESS;
    }

    // 길이의 절반 가져오기
    max_word_len = max((*a)->word_len, (*b)->word_len);
    half_word_len = (max_word_len + 1) >> 1;

    a_sign = (*a)->sign;
    b_sign = (*b)->sign;
    (*a)->sign = 0;
    (*b)->sign = 0;

    // A_1, B_1 계산
    result_msg = bi_shift_right(&a_1, a, half_word_len * WORD_BITS);
    if (result_msg != BI_SHIFT_SUCCESS)
        goto karachuba_exit;
    result_msg = bi_shift_right(&b_1, b, half_word_len * WORD_BITS);
    if (result_msg != BI_SHIFT_SUCCESS)
        goto karachuba_exit;

    // A_0, B_0 계산
    result_msg = bi_mod(&a_0, a, half_word_len * WORD_BITS);
    if (result_msg != BI_MOD_SUCCESS)
        goto karachuba_exit;
    result_msg = bi_mod(&b_0, b, half_word_len * WORD_BITS);
    if (result_msg != BI_MOD_SUCCESS)
        goto karachuba_exit;

    // A_0 * B_0 => a_0b_0 / 분할 정복
    result_msg = bi_mul_karachuba(&a_0b_0, &a_0, &b_0);
    if (result_msg != BI_MUL_SUCCESS)
        goto karachuba_exit;

    //    // A_0 * B_0 => a_0b_0/ 원본 코드
    //    result_msg = bi_mul(&a_0b_0, &a_0, &b_0);
    //    if(result_msg != BI_MUL_SUCCESS)    goto karachuba_exit;

    // A_1 * B_1 => a_1b_1/ 분할 정복
    result_msg = bi_mul_karachuba(&a_1b_1, &a_1, &b_1);
    if (result_msg != BI_MUL_SUCCESS)
        goto karachuba_exit;

    //    // A_1 * B_1 => a_1b_1/ 원본 코드
    //    result_msg = bi_mul(&a_1b_1, &a_1, &b_1);
    //    if(result_msg != BI_MUL_SUCCESS)    goto karachuba_exit;

    // (A_1 * B_1) || (A_0 * B_0) => dst
    result_msg = bi_cat(dst, &a_1b_1, &a_0b_0);
    if (result_msg != BI_CAT_SUCCESS)
        goto karachuba_exit;

    // (A_1 * B_1) + (A_0 * B_0) - (A_1 - A_0) * (B_1 - B_0)
    result_msg = bi_sub(&a_1_a_0, &a_1, &a_0); // A_1 - A_0 => a_1_a_0
    if (result_msg != BI_SUB_SUCCESS)
        goto karachuba_exit;
    result_msg = bi_sub(&b_1_b_0, &b_1, &b_0); // B_1 - B_0 => b_1_b_0
    if (result_msg != BI_SUB_SUCCESS)
        goto karachuba_exit;

    result_msg = bi_add(&a_1b_1, &a_1b_1, &a_0b_0); // (A_1 * B_1) + (A_0 * B_0) => a_1b_1
    if (result_msg != BI_ADD_SUCCESS)
        goto karachuba_exit;

    // (A_1 - A_0) * (B_1 - B_0) => a_1_a_0 / 분할 정복
    result_msg = bi_mul_karachuba(&a_1_a_0, &a_1_a_0, &b_1_b_0);
    if (result_msg != BI_MUL_SUCCESS)
        goto karachuba_exit;

    //    // (A_1 - A_0) * (B_1 - B_0) => a_1_a_0 / 원본 코드
    //    result_msg = bi_mul(&a_1_a_0, &a_1_a_0, &b_1_b_0);
    //    if(result_msg != BI_MUL_SUCCESS)    goto karachuba_exit;

    // (A_1 * B_1) + (A_0 * B_0) - (A_1 - A_0) * (B_1 - B_0) => a_1b_1
    result_msg = bi_sub(&a_1b_1, &a_1b_1, &a_1_a_0);
    if (result_msg != BI_SUB_SUCCESS)
        goto karachuba_exit;

    // ((A_1 * B_1) + (A_0 * B_0) - (A_1 - A_0) * (B_1 - B_0)) << half_word_len * WORD_BITS => a_1b_1
    result_msg = bi_shift_left(&a_1b_1, &a_1b_1, half_word_len * WORD_BITS);
    if (result_msg != BI_SHIFT_SUCCESS)
        goto karachuba_exit;

    // (A_1 * B_1) + ((A_1 * B_1) + (A_0 * B_0) - (A_1 - A_0) * (B_1 - B_0)) + (A_0 * B_0)
    result_msg = bi_add(dst, dst, &a_1b_1);
    if (result_msg != BI_ADD_SUCCESS)
        goto karachuba_exit;

    // 부호 처리
    (*dst)->sign = a_sign ^ b_sign; // XOR 연산으로 부호 처리 다르면 음수, 같으면 양수
    (*a)->sign = a_sign;
    (*b)->sign = b_sign;

    if (bi_refine(*dst) != BI_SET_REFINE_SUCCESS)
        goto karachuba_exit;
    return BI_MUL_SUCCESS;

karachuba_exit:
    if (bi_delete(&a_0) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&b_0) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&a_1) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&b_1) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&a_0b_0) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&a_1b_1) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&a_1_a_0) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&b_1_b_0) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    return result_msg;
}