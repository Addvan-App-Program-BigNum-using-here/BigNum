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

    bigint *tmp = NULL;
    byte carry = 0;
    msg result_msg;
    int max_word_len = 0, sign = 0;

    if (a == NULL || b == NULL)
        return MEM_NOT_ALLOC;
    if (dst != NULL)
    {
        if (bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
    }

    if ((*a)->sign == 0 && (*b)->sign == 1)
    { // a - b
        (*b)->sign = 0;
        return bi_sub(dst, a, b); //  a - b
    }
    if ((*a)->sign == 1 && (*b)->sign == 0)
    { // -a + b = b - a
        (*a)->sign = 0;
        if (bi_sub(dst, b, a) != BI_SUB_SUCCESS)
            return BI_SUB_FAIL; // b - a
        return BI_SUB_SUCCESS;
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
        result_msg = bi_assign(&tmp, b);
    }
    else
    {
        max_word_len = (*b)->word_len;
        result_msg = bi_assign(&tmp, a);
    }

    if (result_msg != BI_SET_ASSIGN_SUCCESS)
        return BI_SET_ASSIGN_FAIL;
    if (bi_expand(&tmp, max_word_len, 0) != BI_EXPAND_SUCCESS)
        return BI_EXPAND_FAIL;

    if (bi_new(dst, max_word_len + 1) != BI_ALLOC_SUCCESS)
        return BI_ALLOC_FAIL;

    // 덧셈 연산 수행
    for (int i = 0; i < max_word_len; i++)
    {
        (*dst)->a[i] = (word)((*a)->a[i] + (*b)->a[i] + carry);
        carry = ((*a)->a[i] > 0xffffffff - ((*b)->a[i] + carry) || (*b)->a[i] > 0xffffffff - ((*a)->a[i] + carry) || ((*a)->a[i] == 0xffffffff && (*b)->a[i] == 0xffffffff)) ? 1 : 0; // carry bit 계산
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
msg bi_sub(OUT bigint **dst, IN bigint **a, IN bigint **b)
{

    bigint *tmp = NULL;
    byte barrow = 0; // 여기 첫 barrow 수행할 때 a >= b일 때 0, a < b 일 때 1
    int sign = 0;

    if (a == NULL || b == NULL)
        return MEM_NOT_ALLOC;
    if (dst != NULL)
    {
        if (bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
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
    if (bi_assign(&tmp, b) != BI_SET_ASSIGN_SUCCESS)
        return BI_SET_ASSIGN_FAIL; // b를 tmp에 복사
    if (bi_expand(&tmp, (*a)->word_len, 0) != BI_EXPAND_SUCCESS)
        return BI_EXPAND_FAIL; // tmp의 길이를 a와 같게 만듦
    if (bi_new(dst, (*a)->word_len) != BI_ALLOC_SUCCESS)
        return BI_ALLOC_FAIL; // 결과값 할당

    // 뺄셈 연산 수행
    for (int i = 0; i < (*a)->word_len; i++)
    {
        (*dst)->a[i] = (word)(barrow * 0xFFFFFFFF - (*b)->a[i] + (*a)->a[i] + 1);
        // barrow 할 때 a = 0이고 barrow가 1인경우 확인 필요
        barrow = ((*a)->a[i] < (*b)->a[i] + barrow) ? 1 : 0; // barrow bit 계산
    }

    bi_delete(&tmp);
    bi_refine(*dst);
    return BI_SUB_SUCCESS;
}

/*************************************************
 * Name:        bi_mul
 *
 * Description: Bigint Multiplication
 *
 * Arguments:   - bigint** dst: pointer to bigint struct
 *              - bigint** a: first operand
 *              - bigint** b: second operand
 **************************************************/

msg bi_mul(OUT bigint **dst, IN bigint **a, IN bigint **b)
{
    // 에러 케이스 정의
    if (a == NULL || b == NULL)
        return MEM_NOT_ALLOC;
    if (dst != NULL)
    {
        if (bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
    }

    int max_len = (*a)->word_len + (*b)->word_len;

    if (bi_new(dst, max_len) != BI_ALLOC_SUCCESS)
        return BI_ALLOC_FAIL;
    // 곱셈은 시작하자마자 sign 으로 부호 판별할 수 있지 않을까?
    (*dst)->sign = (*a)->sign ^ (*b)->sign; // XOR 연산으로 부호 처리 다르면 음수, 같으면 양수
    // 데이터 임시 변수들 선언
    bigint *temp = NULL;
    msg result_msg;

    for (int i = 0; i < (*a)->word_len; i++)
    {
        for (int j = 0; j < (*b)->word_len; j++)
        {
            // 단일 word 결과 저장할 공간
            if (bi_new(&temp, i + j + 2) != BI_ALLOC_SUCCESS)
            {
                bi_delete(dst);
                return BI_ALLOC_FAIL;
            }

            // 16비트 단위로 나누어 곱셈 수행
            word a_low = (*a)->a[i] & 0xFFFF;
            word a_high = (*a)->a[i] >> 16;
            word b_low = (*b)->a[j] & 0xFFFF;
            word b_high = (*b)->a[j] >> 16;

            // 16비트 단위 곱셈 수행
            word p1 = a_low * b_low;   // 하위 16비트 곱
            word p2 = a_low * b_high;  // 중간값 1
            word p3 = a_high * b_low;  // 중간값 2
            word p4 = a_high * b_high; // 상위 16비트 곱
        }
    }

    // 불필요한 상위 0들 제거
    if (bi_refine(*dst) != BI_SET_REFINE_SUCCESS)
        return BI_SET_REFINE_FAIL;

    return BI_MUL_SUCCESS;
}
