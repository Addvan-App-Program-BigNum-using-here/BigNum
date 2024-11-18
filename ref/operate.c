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
    if(*a == NULL || *b == NULL)    return MEM_NOT_ALLOC;
    if((*a)->sign == 0 && (*b)->sign == 1){ // a - b
        (*b)->sign = 0;
        if(bi_sub(dst, a, b) != BI_SUB_SUCCESS)    return BI_ADD_FAIL; // a - b
        if(*dst != *b)  (*b)->sign = 1;
        return BI_ADD_SUCCESS;
    }
    if ((*a)->sign == 1 && (*b)->sign == 0)
    { // -a + b = b - a
        (*a)->sign = 0;
        if(bi_sub(dst, b, a) != BI_SUB_SUCCESS)    return BI_SUB_FAIL; // b - a
        if(*dst != *a)  (*a)->sign = 1;
        return BI_ADD_SUCCESS;
    }
    if ((*a)->sign == 1 && (*b)->sign == 1)
    { // -a + -b = -(a + b)
        (*a)->sign = 0;
        (*b)->sign = 0;
        if(bi_add(dst, a, b) != BI_ADD_SUCCESS)    return BI_ADD_FAIL; // a + b
        if(*dst != *a)  (*a)->sign = 1;
        if(*dst != *b)  (*b)->sign = 1;
        (*dst)->sign = 1; // -(a + b)
        return BI_ADD_SUCCESS;
    }

    byte carry = 0;
    int max_word_len = max((*a)->word_len, (*b)->word_len);
    int dst_word_len = max_word_len + 1;
    word temp_a = 0, temp_b = 0; // dst가 a와 b 중 같은 값일 경우 사용

    if(*dst == NULL){
        if(bi_new(dst, dst_word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if(*dst != NULL && (*dst)->word_len < max_word_len + 1){ // 해당 부분은 나중에 bi_resize 함수 만들어서 최적화 해보자
        if(bi_resize(dst, dst_word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }
    // dst가 a와 b가 아닌 경우 길이는 동일하게 위에서 체크 했으니까 재사용

    // 두 빅넘 크기 맞추기
//    if((*b)->word_len < max_word_len)
//        if(bi_expand(b, max_word_len, 0) != BI_EXPAND_SUCCESS) return BI_EXPAND_FAIL;
//    if((*a)->word_len < max_word_len)
//        if(bi_expand(a, max_word_len, 0) != BI_EXPAND_SUCCESS) return BI_EXPAND_FAIL;
    if(bi_expand(b, max_word_len, 0) != BI_EXPAND_SUCCESS) return BI_EXPAND_FAIL;
    if(bi_expand(a, max_word_len, 0) != BI_EXPAND_SUCCESS) return BI_EXPAND_FAIL;

    // 덧셈 연산 수행
    for(int i = 0; i < max_word_len; i++){
        temp_a = (*a)->a[i];
        temp_b = (*b)->a[i];
        (*dst)->a[i] = (word)(temp_a + temp_b + carry);
        carry = (temp_a > 0xffffffff - (temp_b + carry) || temp_b > 0xffffffff - (temp_a + carry) || (temp_a == 0xffffffff && temp_b == 0xffffffff)) ? 1 : 0; // carry bit 계산
    }
    if (carry)
        (*dst)->a[max_word_len] = carry;

    if(bi_resize(dst, dst_word_len) != BI_RESIZE_SUCCESS){
        if(bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
        return BI_RESIZE_FAIL;
    }
//    if(bi_refine(dst) != BI_SET_REFINE_SUCCESS){
//        if(bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
//        return BI_SET_REFINE_FAIL;
//    }
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
    if(*a == NULL || *b == NULL)    return MEM_NOT_ALLOC;

    if ((*a)->sign == 0 && (*b)->sign == 1)
    { // a - (-b) = a + b
        (*b)->sign = 0;
        if(bi_add(dst, a, b) != BI_ADD_SUCCESS)    return BI_SUB_FAIL;
        if(*dst != *b)  (*b)->sign = 1;
        return BI_SUB_SUCCESS;
    }
    else if ((*a)->sign == 1 && (*b)->sign == 0)
    { // -a - b = -(a + b)
        (*a)->sign = 0;
        if(bi_add(dst, a, b) != BI_ADD_SUCCESS)    return BI_SUB_FAIL;
        if(*dst != *a)  (*a)->sign = 1;
        (*dst)->sign = 1;
        return BI_SUB_SUCCESS;
    }
    else if ((*a)->sign == 1 && (*b)->sign == 1)
    { // -a - (-b) = b - a
        (*a)->sign = 0;
        (*b)->sign = 0;
        if(bi_sub(dst, b, a) != BI_SUB_SUCCESS)    return BI_SUB_FAIL;
        if(*dst != *a)  (*a)->sign = 1;
        if(*dst != *b)  (*b)->sign = 1;
        return BI_SUB_SUCCESS;
    }

    // b가 a보다 더 큰 경우 (a - b = -(b - a))
    if(bi_compare_abs(a, b) == -1){
        // b - a
        if (bi_sub(dst, b, a) != BI_SUB_SUCCESS)
            return BI_SUB_FAIL;
        (*dst)->sign = 1; // -(b - a) // 모듈러 체크
        return BI_SUB_SUCCESS;
    }

    int max_word_len = (*a)->word_len;
    byte borrow = bi_compare_abs(a, b) >= 0 ? 0 : 1; // 여기 첫 borrow 수행할 때 a >= b일 때 0, a < b 일 때 1
    word temp_a = 0, temp_b = 0; // dst가 a와 b 중 같은 값일 경우 사용

    if(*dst == NULL){
        if(bi_new(dst, max_word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if(*dst != NULL && (*dst)->word_len < max_word_len){ // 해당 부분은 나중에 bi_resize 함수 만들어서 최적화 해보자
        if(bi_resize(dst, max_word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }
    // dst가 a와 b가 아닌 경우 길이는 동일하게 위에서 체크 했으니까 재사용

    // 상수 시간 연산을 위해 a와 b의 크기를 맞춰서 연산 수행 -> 무조건 a >= b
    if(bi_expand(b, max_word_len, 0) != BI_EXPAND_SUCCESS)    return BI_EXPAND_FAIL;
    // 뺄셈 연산 수행
    for(int i = 0; i < (*a)->word_len; i++){
        temp_a = (*a)->a[i];
        temp_b = (*b)->a[i];
        (*dst)->a[i] = (word)(borrow * 0xFFFFFFFF - temp_b + temp_a);
        borrow = ((temp_a < temp_b + borrow) || (temp_b == 0xffffffff && borrow)) ? 1 : 0; // borrow bit 계산
    }
    // 부호 설정
    (*dst)->sign = 0;
    if(bi_resize(dst, max_word_len) != BI_RESIZE_SUCCESS){
        if(bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
        return BI_RESIZE_FAIL;
    }
//    if(bi_refine(dst) != BI_SET_REFINE_SUCCESS){
//        if(bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
//        return BI_SET_REFINE_FAIL;
//    }
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
msg bi_mul(OUT bigint **dst, IN bigint **a, IN bigint **b){
    // 나중에 자료형 단위가 달라져도 동작할 수 있게 수정하기
    if(*a == NULL || *b == NULL)    return MEM_NOT_ALLOC;

    msg result_msg = 0;
    if(bi_compare_abs(a, b) == -1){ // a < b
        result_msg = bi_mul(dst, b, a);
        if(result_msg != BI_MUL_SUCCESS)    return result_msg;
        return BI_MUL_SUCCESS;
    }

    bigint* temp1 = NULL;
    bigint* temp2 = NULL;
    bigint* dst_temp = NULL;
    word b_i;
    int temp_len_a = (*a)->word_len;
    int temp_len_b = (*b)->word_len;
    int idx_temp1, idx_temp2;
    byte a_sign, b_sign;
    int max_word_len = (*a)->word_len + (*b)->word_len;

    if(bi_new(&dst_temp, max_word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;

    result_msg = bi_new(&(temp1), temp_len_a);
    if(result_msg != BI_ALLOC_SUCCESS)  goto MUL_EXIT;
    result_msg = bi_new(&(temp2), temp_len_a);
    if(result_msg != BI_ALLOC_SUCCESS)  goto MUL_EXIT;

    a_sign = (*a)->sign;
    b_sign = (*b)->sign;
    (*a)->sign = 0;
    (*b)->sign = 0;

    // 반복문 밖에가 작은 값, 안쪽이 큰 값
    for (int i = (temp_len_b * 2) - 1; i >= 0; i--){ // word_len == 2
        b_i = (i % 2 == 0) ? (*b)->a[i / 2] & 0xFFFF : (*b)->a[i / 2] >> 16; // 16비트 단위로 나누어 곱셈 수행
        idx_temp1 = 0;
        idx_temp2 = 0;
        // temp1, temp2 초기화
        for(int j = 0; j < temp1->word_len; j++)    temp1->a[j] = 0;
        for(int j = 0; j < temp2->word_len; j++)    temp2->a[j] = 0;

        // 곱셈 수행
        for (int j = 0; j < temp_len_a * 2; j++){ // word_len == 2
            if(j % 2 == 0)  (*temp1).a[idx_temp1++] = (word)(b_i * ((*a)->a[j / 2] & 0xFFFF)); // 하위 16비트 곱
            else    (*temp2).a[idx_temp2++] = (word)(b_i * ((*a)->a[j / 2] >> 16)); // 상위 16비트 곱
        }

        result_msg = bi_shift_left(&temp1, &temp1, i * 16); // 16비트 단위로 밀기
        if (result_msg != BI_SHIFT_SUCCESS)
            goto MUL_EXIT;
        result_msg = bi_shift_left(&temp2, &temp2, (i + 1) * 16); // 16비트 단위로 밀기
        if (result_msg != BI_SHIFT_SUCCESS)
            goto MUL_EXIT;

        result_msg = bi_add(&temp1, &temp1, &temp2); // 덧셈 수행
        if(result_msg != BI_ADD_SUCCESS)  goto MUL_EXIT;
        result_msg = bi_add(&dst_temp, &dst_temp, &temp1); // 덧셈 수행
        if(result_msg != BI_ADD_SUCCESS)  goto MUL_EXIT;
    }

    // 부호 처리
    dst_temp->sign = a_sign ^ b_sign; // XOR 연산으로 부호 처리 다르면 음수, 같으면 양수
    (*a)->sign = a_sign;
    (*b)->sign = b_sign;

    if(bi_assign(dst, &dst_temp) != BI_SET_ASSIGN_SUCCESS)    goto MUL_EXIT;
    if (bi_refine(dst) != BI_SET_REFINE_SUCCESS)   goto MUL_EXIT;
    result_msg = BI_MUL_SUCCESS;

MUL_EXIT:
    if (bi_delete(&temp1) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&temp2) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
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
msg bi_mul_karachuba(OUT bigint **dst, IN bigint **a, IN bigint **b){
    if (*a == NULL || *b == NULL) return MEM_NOT_ALLOC;
    msg result_msg = 0;
    if(bi_refine(a) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;
    if(bi_refine(b) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;

    // base case에서 카라츄바가 아닌 일반 곱셈 수행을 위한 연산
    int min_word_len = min((*a)->word_len, (*b)->word_len);
    if(flag > min_word_len){
        result_msg = bi_mul(dst, a, b);
        if(result_msg != BI_MUL_SUCCESS)    return result_msg;
        return BI_MUL_SUCCESS;
    }

    bigint* a_0 = g_pool.pool[g_pool.current_depth][0];
    bigint* b_0 = g_pool.pool[g_pool.current_depth][1];
    bigint* a_1 = g_pool.pool[g_pool.current_depth][2];
    bigint* b_1 = g_pool.pool[g_pool.current_depth][3];
    bigint* a_0b_0 = g_pool.pool[g_pool.current_depth][4];
    bigint* a_1b_1 = g_pool.pool[g_pool.current_depth][5];
    bigint* a_1_a_0 = g_pool.pool[g_pool.current_depth][6];
    bigint* b_1_b_0 = g_pool.pool[g_pool.current_depth][7];

    g_pool.current_depth++;

    byte a_sign = 0, b_sign = 0;
    int dst_word_len = (*a)->word_len + (*b)->word_len;
    int max_word_len = max((*a)->word_len, (*b)->word_len); // 길이의 절반 가져오기
    int half_word_len = (max_word_len + 1) >> 1;

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

    result_msg = bi_mul_karachuba(&a_0b_0, &a_0, &b_0);
    if(result_msg != BI_MUL_SUCCESS)    goto karachuba_exit;
    result_msg = bi_mul_karachuba(&a_1b_1, &a_1, &b_1);
    if (result_msg != BI_MUL_SUCCESS)
        goto karachuba_exit;

    if(*dst == NULL){
        if(bi_new(dst, dst_word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if(*dst != NULL && (*dst)->word_len < max_word_len){
        if(bi_resize(dst, dst_word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }

    // (A_1 * B_1) || (A_0 * B_0) => dst
    result_msg = bi_cat(dst, &a_1b_1, &a_0b_0);
    if (result_msg != BI_CAT_SUCCESS)
        goto karachuba_exit;

    // (A_1 * B_1) + (A_0 * B_0) - (A_1 - A_0) * (B_1 - B_0)
    result_msg = bi_sub(&a_1_a_0, &a_1, &a_0); // A_1 - A_0 => a_1_a_0
    if(result_msg != BI_SUB_SUCCESS)    goto karachuba_exit;

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
    if(*dst != *a)  (*a)->sign = a_sign;
    if(*dst != *b)  (*b)->sign = b_sign;

    if(bi_resize(dst, dst_word_len) != BI_RESIZE_SUCCESS)    goto karachuba_exit;
    result_msg = BI_MUL_SUCCESS;

karachuba_exit:
    g_pool.current_depth--;
    return result_msg;
}

/*************************************************
* Name:        init_karachuba_pool
*
* Description: initialize Karachuba pool
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - bigint** a: bigint struct
*              - bigint** b: bigint struct
**************************************************/
msg init_karachuba_pool(int max_word_size){
    if (g_pool.initialized) return INIT_KARACHUBA_POOL_SUCCESS;
    g_pool.max_word_size = max_word_size;
    g_pool.current_depth = 0;

    // 각 레벨별로 메모리 할당
    for (int depth = 0; depth < MAX_RECURSION_DEPTH; depth++) {
        for (int idx = 0; idx < POOL_SIZE; idx++) {
            if (bi_new(&g_pool.pool[depth][idx], max_word_size) != BI_ALLOC_SUCCESS) {
                // 실패시 이전까지 할당된 메모리 해제
                for (int d = depth; d >= 0; d--) {
                    for (int i = (d == depth ? idx - 1 : POOL_SIZE - 1); i >= 0; i--) {
                        bi_delete(&g_pool.pool[d][i]);
                    }
                }
                return INIT_KARACHUBA_POOL_FAIL;
            }
        }
    }

    g_pool.initialized = true;
    return INIT_KARACHUBA_POOL_SUCCESS;
}

/*************************************************
* Name:        clear_karachuba_pool
*
* Description: clear Karachuba pool
*
**************************************************/
msg clear_karachuba_pool(){
    if (!g_pool.initialized) return CLEAR_KARACHUBA_POOL_SUCCESS;

    for (int depth = 0; depth < MAX_RECURSION_DEPTH; depth++) {
        for (int idx = 0; idx < POOL_SIZE; idx++) {
            if (bi_delete(&g_pool.pool[depth][idx]) != BI_FREE_SUCCESS) {
                return CLEAR_KARACHUBA_POOL_FAIL;
            }
        }
    }

    g_pool.initialized = false;
    return CLEAR_KARACHUBA_POOL_SUCCESS;
}

/*************************************************
 * Name:        bi_div
 *
 * Description: Bigint division
 *
 * Arguments:   - bigint** q: pointer to bigint struct
 *              - bigint** r: pointer to bigint struct
 *              - bigint** a: bigint struct
 *              - bigint** b: bigint struct
 **************************************************/
msg bi_div(OUT bigint **q, OUT bigint **r, IN bigint **a, IN bigint **b)
{
    msg result_msg;

    // 1. NULL 체크 강화
    if (q == NULL || r == NULL || a == NULL || b == NULL ||
        *a == NULL || *b == NULL)
        return MEM_NOT_ALLOC;

    // 2. 0으로 나누기 체크
    if ((*b)->word_len == 1 && (*b)->a[0] == 0)
        return BI_DIV_FAIL;

    // 3. 기존 메모리 해제
    if (*q != NULL)
    {
        if (bi_delete(q) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
        *q = NULL;
    }

    if (*r != NULL)
    {
        if (bi_delete(r) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
        *r = NULL;
    }

    // 4. |a| < |b| 케이스 처리
    if (bi_compare_abs(a, b) < 0)
    {
        if (bi_new(q, 1) != BI_ALLOC_SUCCESS)
            return BI_ALLOC_FAIL;
        (*q)->a[0] = 0;

        if (bi_new(r, (*a)->word_len) != BI_ALLOC_SUCCESS)
        {
            bi_delete(q);
            return BI_ALLOC_FAIL;
        }

        result_msg = bi_assign(r, a);
        if (result_msg != BI_SET_ASSIGN_SUCCESS)
        {
            bi_delete(q);
            bi_delete(r);
            return result_msg;
        }

        (*q)->sign = (*a)->sign ^ (*b)->sign;
        return BI_DIV_SUCCESS;
    }

    // 5. 나눗셈을 위한 메모리 할당
    int q_wordlen = (*a)->word_len - (*b)->word_len + 1;
    if (q_wordlen <= 0)
        return BI_DIV_FAIL;

    if (bi_new(q, q_wordlen) != BI_ALLOC_SUCCESS)
        return BI_DIV_FAIL;

    if (bi_new(r, (*a)->word_len) != BI_ALLOC_SUCCESS)
    {
        bi_delete(q);
        return BI_ALLOC_FAIL;
    }

    result_msg = bi_assign(r, a);
    if (result_msg != BI_SET_ASSIGN_SUCCESS)
    {
        bi_delete(q);
        bi_delete(r);
        return result_msg;
    }

    // 6. 임시 변수 선언
    bigint *temp_sub = NULL;
    bigint *temp_b = NULL;
    bigint *temp_q = NULL;
    word b_sign = (*b)->sign;
    (*b)->sign = 0;

    // 7. 개선된 나눗셈 수행
    for (int i = (*r)->word_len - (*b)->word_len; i >= 0; i--)
    {
        if (temp_b != NULL)
        {
            bi_delete(&temp_b);
        }

        // 현재 위치에서의 temporary divisor 계산
        if (bi_new(&temp_b, (*b)->word_len + i) != BI_ALLOC_SUCCESS)
            goto DIV_EXIT;

        result_msg = bi_shift_left(&temp_b, b, i * WORD_BITS);
        if (result_msg != BI_SHIFT_SUCCESS)
            goto DIV_EXIT;

        // Binary search for quotient digit
        word left = 0, right = UINT32_MAX;
        word mid = 0;

        while (left <= right)
        {
            mid = (left + right) >> 1;

            // Calculate temp_q = mid * temp_b
            if (bi_new(&temp_q, temp_b->word_len + 1) != BI_ALLOC_SUCCESS)
                goto DIV_EXIT;

            for (int j = 0; j < temp_b->word_len; j++)
            {
                temp_q->a[j] = temp_b->a[j] * mid;
            }

            int cmp = bi_compare_abs(r, &temp_q);

            if (cmp == 0)
            {
                (*q)->a[i] = mid;
                bi_assign(r, &temp_q);
                break;
            }
            else if (cmp > 0)
            {
                left = mid + 1;
                (*q)->a[i] = mid;

                result_msg = bi_sub(&temp_sub, r, &temp_q);
                if (result_msg != BI_SUB_SUCCESS)
                    goto DIV_EXIT;

                result_msg = bi_assign(r, &temp_sub);
                if (result_msg != BI_SET_ASSIGN_SUCCESS)
                    goto DIV_EXIT;
            }
            else
            {
                right = mid - 1;
            }

            bi_delete(&temp_q);
            if (temp_sub != NULL)
            {
                bi_delete(&temp_sub);
                temp_sub = NULL;
            }

            if (left > right)
                break;
        }
    }

    // 8. 부호 처리
    (*b)->sign = b_sign;
    (*q)->sign = (*a)->sign ^ (*b)->sign;
    (*r)->sign = (*a)->sign;

    if (bi_refine(*q) != BI_SET_REFINE_SUCCESS)
        goto DIV_EXIT;
    if (bi_refine(*r) != BI_SET_REFINE_SUCCESS)
        goto DIV_EXIT;

    return BI_DIV_SUCCESS;

DIV_EXIT:
    (*b)->sign = b_sign;
    if (temp_sub != NULL)
        bi_delete(&temp_sub);
    if (temp_b != NULL)
        bi_delete(&temp_b);
    if (temp_q != NULL)
        bi_delete(&temp_q);
    bi_delete(q);
    bi_delete(r);
    return BI_DIV_FAIL;