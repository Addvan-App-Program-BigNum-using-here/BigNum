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
    if ((*a)->sign == 1 && (*b)->sign == 0){ // -a + b = b - a
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
    if(bi_expand(b, max_word_len, 0) != BI_EXPAND_SUCCESS) return BI_EXPAND_FAIL;
    if(bi_expand(a, max_word_len, 0) != BI_EXPAND_SUCCESS) return BI_EXPAND_FAIL;

    // 덧셈 연산 수행
    for(int i = 0; i < max_word_len; i++){
        temp_a = (*a)->a[i];
        temp_b = (*b)->a[i];
        (*dst)->a[i] = (word)(temp_a + temp_b + carry);
        carry = (temp_a > 0xffffffff - (temp_b + carry) || temp_b > 0xffffffff - (temp_a + carry) || (temp_a == 0xffffffff && temp_b == 0xffffffff)) ? 1 : 0; // carry bit 계산
    }
    if (carry)  (*dst)->a[max_word_len] = carry;
    (*dst)->sign = 0;

    if(bi_resize(dst, dst_word_len - (carry ^ 1)) != BI_RESIZE_SUCCESS){ // carry ^ 1을 한 이유는 마지막 carry가 발생하지 않으면 사이즈를 1 word 줄여도 되서
        if(bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
        return BI_RESIZE_FAIL;
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
msg bi_sub(OUT bigint** dst, IN bigint** a, IN bigint** b){
    if(*a == NULL || *b == NULL)    return MEM_NOT_ALLOC;

    if ((*a)->sign == 0 && (*b)->sign == 1){ // a - (-b) = a + b
        (*b)->sign = 0;
        if(bi_add(dst, a, b) != BI_ADD_SUCCESS)    return BI_SUB_FAIL;
        if(*dst != *b)  (*b)->sign = 1;
        return BI_SUB_SUCCESS;
    }
    else if ((*a)->sign == 1 && (*b)->sign == 0){ // -a - b = -(a + b)
        (*a)->sign = 0;
        if(bi_add(dst, a, b) != BI_ADD_SUCCESS)    return BI_SUB_FAIL;
        if(*dst != *a)  (*a)->sign = 1;
        (*dst)->sign = 1;
        return BI_SUB_SUCCESS;
    }
    else if ((*a)->sign == 1 && (*b)->sign == 1){ // -a - (-b) = b - a
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

    // 여기서 첫 barrow 계산할 때 a랑 b를 전체를 넣으면 안되고 a[0] b[0]끼리 비교를 해줘야 한다.
    byte borrow = (*a)->a[0] >= (*b)->a[0] ? 0 : 1; // 여기 첫 borrow 수행할 때 a >= b일 때 0, a < b 일 때 1
    byte borrow_temp = 0; // 이전 word에서 사용한 borrow 값을 뺄 때 사용

    if(*dst == NULL){
        if(bi_new(dst, max_word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if(*dst != NULL && (*dst)->word_len < max_word_len){ // 해당 부분은 나중에 bi_resize 함수 만들어서 최적화 해보자
        if(bi_resize(dst, max_word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }
    // dst가 a와 b가 아닌 경우 길이는 동일하게 위에서 체크 했으니까 재사용

    // 상수 시간 연산을 위해 a와 b의 크기를 맞춰서 연산 수행 -> 무조건 a >= b
    if(bi_expand(b, max_word_len, 0) != BI_EXPAND_SUCCESS)    return BI_EXPAND_FAIL;
    // 뺄셈 연산 수행
    for(int i = 0; i < (*a)->word_len - 1; i++){
        // borrow를 사용할 경우 0x100000000을 더해줘야 하는데 이를 0xffffffff + 0x1로 나누어 더해 준다. 그리고 이전에 빌려갔던 borrow_temp를 빼준다.
        (*dst)->a[i] = (word)(borrow * 0xFFFFFFFF - (*b)->a[i] + (*a)->a[i] + borrow - borrow_temp);
        borrow_temp = borrow;
        borrow = ((*a)->a[i + 1] < (*b)->a[i + 1] || ((*b)->a[i + 1] == 0xffffffff && borrow)) ? 1 : 0; // borrow bit 계산
    }
    (*dst)->a[max_word_len - 1] = (word)(borrow * 0xFFFFFFFF - (*b)->a[max_word_len - 1] + (*a)->a[max_word_len - 1] + borrow - borrow_temp);
    // 부호 설정
    (*dst)->sign = 0;
    if(bi_resize(dst, max_word_len) != BI_RESIZE_SUCCESS){
        if(bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
        return BI_RESIZE_FAIL;
    }

    return BI_SUB_SUCCESS;
}

msg bi_mul_word(OUT bigint **dst, IN word a, IN word b){
    if(*dst == NULL){
        if(bi_new(dst, 2) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if((*dst)->word_len != 2){
        if(bi_resize(dst, 2) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }

    (*dst)->a[0] = a * b;
    (*dst)->a[1] = 0;
    (*dst)->sign = 0;

    return BI_MUL_SUCCESS;
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

    msg result_msg = BI_MUL_FAIL;
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
            if(j % 2 == 0)  temp1->a[idx_temp1++] = (word)(b_i * ((*a)->a[j / 2] & 0xFFFF)); // 하위 16비트 곱
            else    temp2->a[idx_temp2++] = (word)(b_i * ((*a)->a[j / 2] >> 16)); // 상위 16비트 곱
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
    msg result_msg = BI_MUL_FAIL;

    if(bi_refine(a) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;
    if(bi_refine(b) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;

    if(bi_compare_abs(a, b) == -1){ // a < b
        result_msg = bi_mul_karachuba(dst, b, a);
        if(result_msg != BI_MUL_SUCCESS)    return result_msg;
        return BI_MUL_SUCCESS;
    }

    // base case에서 카라츄바가 아닌 일반 곱셈 수행을 위한 연산
    int min_word_len = min((*a)->word_len, (*b)->word_len);
    if(karachuba_flag > min_word_len){
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

    if(bi_resize(b, max_word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
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
    result_msg = bi_get_lower(&a_0, a, half_word_len * WORD_BITS);
    if (result_msg != BI_GET_LOWER_SUCCESS)
        goto karachuba_exit;

    result_msg = bi_get_lower(&b_0, b, half_word_len * WORD_BITS);
    if (result_msg != BI_GET_LOWER_SUCCESS)
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
msg bi_div(OUT bigint **q, OUT bigint **r, IN bigint **a, IN bigint **b){
    if(*a == NULL || *b == NULL)    return MEM_NOT_ALLOC;
    // 몫과 나머지가 같은 메모리를 가리키면 안됨
    if(*q != NULL && *q == *r)    return DIV_FAIL;

    msg result_msg = BI_DIV_FAIL;
    bigint* temp_r = NULL;

    // b가 0이거나 할당이 안되었을 경우
    result_msg = bi_is_zero(b);
    if(result_msg == BI_NOT_USING || result_msg == BI_IS_ZERO){
        log_msg(result_msg);
        return BI_DIV_BY_ZERO;
    }

    if((*b)->sign){ // b가 음수인 경우
        if((*a)->sign){ // a, b 모두 음수인 경우
            (*a)->sign = 0;
            (*b)->sign = 0;
            result_msg = bi_div(q, r, a, b);
            if(result_msg != BI_DIV_SUCCESS)    return result_msg;
            (*a)->sign = 1;
            (*b)->sign = 1;
            (*r)->sign = 1;
            result_msg = BI_DIV_SUCCESS;
            goto EXIT_DIV_REFINE;
        }
        // a가 양수인 경우
        (*b)->sign = 0; // b는 양수로
        (*a)->sign = 1; // a는 음수로
        result_msg = bi_div(q, r, a, b);
        if(result_msg != BI_DIV_SUCCESS)    return result_msg;
        (*a)->sign = 0;
        (*b)->sign = 1;
        result_msg = BI_DIV_SUCCESS;
        goto EXIT_DIV_REFINE;
    }else if((*a)->sign){ // a가 음수, b가 양수인 경우
        // 1 만들기
        bigint* one = NULL;
        if(bi_new(&one, 1) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
        one->a[0] = 1;

        (*a)->sign = 0;
        result_msg = bi_div(q, r, a, b);
        if(result_msg != BI_DIV_SUCCESS)    return result_msg;
        (*a)->sign = 1;

        // q와 r 처리
        if(bi_sub(r, b, r) != BI_SUB_SUCCESS)    return BI_SUB_FAIL; // r = b - r 수행. 여기서 나눗셈 연산 과정에서 b가 변하면 안된다!

        (*q)->sign = 0; // -q
        if(bi_add(q, q, &one) != BI_ADD_SUCCESS)    return BI_ADD_FAIL; // q = -q - 1 수행 // 일단 여기서 문제 발생
        (*q)->sign = 1; // q = -q - 1
        result_msg = BI_DIV_SUCCESS;
        goto EXIT_DIV_REFINE;
    }

    // 해당 코드로 짤 경우 time attack 가능성이 있음
    int bi_compare = bi_compare_abs(a, b);
    if(bi_compare <= 0){
        if(*q == NULL){
            if(bi_new(q, 1) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
        }else{
            if(bi_resize(q, 1) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
        }
        (*q)->word_len = 1;
        if(!bi_compare){
            (*q)->a[0] = 1;
            (*q)->sign = ((*a)->sign ^ (*b)->sign) ? 1 : 0; // 부호가 다르면 음수, 같으면 양수
            if(*r == NULL){
                if(bi_new(r, 1) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
            }else{
                if(bi_resize(r, 1) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
            }
            (*r)->word_len = 1;
            (*r)->a[0] = 0;
        }else{
            (*q)->a[0] = (word)(*a)->sign;
            (*q)->sign = (*a)->sign;
            // r 메모리 할당
            if(*r == NULL){
                if(bi_new(r, (*a)->word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
            }else if(*r != NULL && (*r) != (*a)){
                if(bi_resize(r, (*a)->word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
            }
            // a가 음수인 경우 r = b + a, a가 양수인 경우 r = a
            if(bi_assign(r, a) != BI_SET_ASSIGN_SUCCESS)    return BI_ALLOC_FAIL; // 어차피 *r == *a 인경우 assign에서 바로 return

            if((*a)->sign){ // a가 음수인 경우
                if(bi_add(r, r, b) != BI_ADD_SUCCESS)    return BI_ADD_FAIL;
            }
        }
        result_msg = BI_DIV_SUCCESS;
        goto EXIT_DIV_REFINE;
    }

    // 나눗셈을 위한 메모리 할당
    int max_q_size = (*a)->word_len; // b가 음수가 아닐 경우
    bigint* temp_q = NULL;

    if(bi_new(&temp_q, max_q_size) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;

    // r의 여부 상관 없이 무조건 할당하는 것이 더 효율적일 듯. a, b랑 같을 때만 할 수 는 있는데 코드가 너무 더러워짐
    result_msg = bi_new(&temp_r, (*b)->word_len + 1);
    if(result_msg != BI_ALLOC_SUCCESS)    goto EXIT_DIV;

    result_msg = divc(&temp_q, &temp_r, a, b);
    if(result_msg != DIVC_SUCCESS)  goto EXIT_DIV;

    if(*q == NULL){
        result_msg = bi_new(q, (*b)->word_len);
        if(result_msg != BI_ALLOC_SUCCESS)    goto EXIT_DIV;
    }
    result_msg = bi_assign(q, &temp_q);
    if(result_msg != BI_SET_ASSIGN_SUCCESS)    goto EXIT_DIV;
    // r이 NULL이면 할당
    if(*r == NULL){
        result_msg = bi_new(r, (*b)->word_len);
        if(result_msg != BI_ALLOC_SUCCESS)    goto EXIT_DIV;
    }
    result_msg = bi_refine(&temp_r);
    if(result_msg != BI_SET_REFINE_SUCCESS)   goto EXIT_DIV;
    result_msg = bi_assign(r, &temp_r);
    if(result_msg != BI_SET_ASSIGN_SUCCESS)    goto EXIT_DIV;
    result_msg = BI_DIV_SUCCESS;

EXIT_DIV:
    if(bi_delete(&temp_r) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&temp_q) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
EXIT_DIV_REFINE:
    if(bi_refine(r) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;
    if(bi_refine(q) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;
    return result_msg;
}

/*************************************************
 * Name:        divc
 *
 * Description: Bigint division using binary division
 *
 * Arguments:   - bigint** q: pointer to bigint struct
 *              - bigint** r: pointer to bigint struct
 *              - bigint** a: bigint struct
 *              - bigint** b: bigint struct
 **************************************************/
msg divc(OUT bigint** q, OUT bigint** r, IN bigint** a, IN bigint** b){
    if(*a == NULL || *b == NULL)    return MEM_NOT_ALLOC;
    for(int i = (*a)->word_len - 1; i >= 0; i--){
        for(int j = WORD_BITS - 1; j >= 0; j--){ // bit 단위의 곱셈을 수행하기에
            // R <- R << 1 | (a[i] >> j & 1) -> 이 버전은 bit division 버전이다.
            bi_shift_left(r, r, 1); // r <- r << 1
            (*r)->a[0] |= (((*a)->a[i] >> j) & 1); // r <- r | (a[i] >> j & 1)
            // if R >= b then R <- R - b, Q[i] <- 1
            if(bi_compare_abs(r, b) >= 0){ // 여기서 r이 재활용 되지 않은 경우는 상관 없는데 재활용 하는 경우 쓰레기 값이 있는지 확인해볼 필요가 있다. 내부에서 refine 해주기는 한다.
                if(bi_sub(r, r, b) != BI_SUB_SUCCESS)    return DIVC_FAIL;
                (*r)->a[(*r)->word_len - 1] = 0;
                (*q)->a[i] |= 1 << j;
            }
        }
    }
    return DIVC_SUCCESS;
}

///*************************************************
// * Name:        divc_gener
// *
// * Description: Bigint division using word division
// *
// * Arguments:   - bigint** q: pointer to bigint struct
// *              - bigint** r: pointer to bigint struct
// *              - bigint** a: bigint struct
// *              - bigint** b: bigint struct
// **************************************************/
//msg divc_gener(OUT bigint** q, OUT bigint** r, IN bigint** a, IN bigint** b){
//    if(*a == NULL || *b == NULL)    return MEM_NOT_ALLOC;
//
//    word Q_hat = 0;
//
//    Q_hat =
//}

/*************************************************
 * Name:        bi_squ
 *
 * Description: Bigint squaring
 *
 * Arguments:   - bigint** dst: pointer to bigint struct
 *              - bigint** a: bigint struct to be squared
 **************************************************/
// 데이터 넣을 때 조금 더 최적화 할 수 있는 방법 생각해보기
msg bi_squ(OUT bigint** dst, IN bigint** a){
    if(*a == NULL)    return MEM_NOT_ALLOC;
    msg result_msg = BI_SQU_FAIL;
    word T2 = 0; // Ai * Aj 결과 저장
    word temp = 0; // Aj 저장
    bigint* C1 = NULL;  // 제곱 결과 저장
    bigint* C2 = NULL;  // Ai * Aj 홀수 내용 저장
    bigint* C3 = NULL;  // Ai * Aj 짝수 내용 저장
    bigint* mid = NULL; // C1과 C2의 중간 결과 값 저장
    int max_word_len = (*a)->word_len * 2;

    result_msg = bi_new(&C1, max_word_len);
    if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
    result_msg = bi_new(&C2, max_word_len);
    if(result_msg != BI_ALLOC_SUCCESS) goto EXIT_SQU;
    result_msg = bi_new(&C3, max_word_len);
    if(result_msg != BI_ALLOC_SUCCESS) goto EXIT_SQU;
    result_msg = bi_new(&mid, max_word_len);
    if(result_msg != BI_ALLOC_SUCCESS) goto EXIT_SQU;

    for(int j = 0; j < max_word_len; j++){
        C3->a[j] = 0; // C3 초기화 -> 처음에 j 번째만 초기화 하면 나머지는 덮어써짐
        if(j % 2 == 0)  temp = ((*a)->a[j / 2]) & 0xffff;
        else if(j % 2 == 1)    temp = ((*a)->a[j / 2]) >> 16;
        C1 -> a[j] = temp * temp;
        for(int i = j + 1; i < max_word_len; i++){
            if(i % 2 == 1)  T2 = (temp * (((*a)->a[i / 2]) >> 16)); // 위에서 연산했던 temp 재사용
            else if(i % 2 == 0)    T2 = temp * (((*a) -> a[i / 2]) & 0xffff);
            if((i + j) % 2 == 0)    C3 ->a[(i + j) / 2] = T2; // 짝수
            else C2 -> a[(i + j) / 2] = T2; // 홀수
        }
        result_msg = bi_shift_left(&C2, &C2, 16); // 16비트 단위로 밀기
        if(result_msg != BI_SHIFT_SUCCESS) goto EXIT_SQU;
        result_msg = bi_add(&mid, &mid, &C2);
        if(result_msg != BI_ADD_SUCCESS) goto EXIT_SQU;
        result_msg = bi_add(&mid, &mid, &C3);
        if(result_msg != BI_ADD_SUCCESS) goto EXIT_SQU;
        // C2의 경우 j와 사용 마지막 인덱스 + 1만 초기화 해주면 나머지는 덮어써진다.
        C2->a[j] = 0;
        C2->a[(j + max_word_len) / 2] = 0;
    }

    result_msg = bi_shift_left(dst, &mid, 1); // 2배 만들기
    if(result_msg != BI_SHIFT_SUCCESS) goto EXIT_SQU;

    result_msg = bi_add(dst, dst, &C1); // 제곱 더하기
    if(result_msg != BI_ADD_SUCCESS) goto EXIT_SQU;

    result_msg = bi_refine(dst);
    if(result_msg != BI_SET_REFINE_SUCCESS) goto EXIT_SQU;

    (*dst) -> sign = 0; // 부호 설정

    result_msg = BI_SQU_SUCCESS;

EXIT_SQU:
    if(bi_delete(&C1) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&C2) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&C3) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&mid) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;

    return result_msg;
}

/*************************************************
 * Name:        bi_squ_karachuba
 *
 * Description: Bigint squaring by using karachuba algorithm
 *
 * Arguments:   - bigint** dst: pointer to bigint struct
 *              - bigint** a: bigint struct to be squared
 **************************************************/
//msg bi_squ_karachuba(OUT bigint** dst, IN bigint** a){
//    if(*a == NULL)  return MEM_NOT_ALLOC;
//    msg result_msg = BI_SQU_FAIL;
//
//    // base case에서 카라츄바가 아닌 일반 곱셈 수행을 위한 연산
//    if(squ_karachuba_flag > (*a)->word_len){
//        result_msg = bi_squ(dst, a);
//        if(result_msg != BI_SQU_SUCCESS)    return result_msg;
//        return BI_SQU_SUCCESS;
//    }
//    l = (*a)->word_len;
//}

/*************************************************
 * Name:        bi_exp_MS
 *
 * Description: Bigint modular exponentiation using Montgomery Square
 *
 * Arguments:   - bigint** dst: pointer to bigint struct
 *              - bigint** src: bigint struct to be squared
 *              - bigint** x: bigint struct square
 *             - bigint** n: bigint struct modulus
 **************************************************/
msg bi_exp_MS(OUT bigint** dst, IN bigint** src, IN bigint** x, IN bigint** n){
    if(*src == NULL || *n == NULL)    return MEM_NOT_ALLOC;

    // 음수인 경우 -> 이거는 역원 찾는거라서 일단 패스 -> 이거 test에서 전처리 함
    if((*x)->sign)    return BI_EXP_MS_FAIL;
    // 0인 경우 time attack을 방지하고자 0값 할당해서 수행하자.
    if(bi_is_zero(x) == BI_IS_ZERO){
        (*x)->word_len = 1;
        (*x)->a[0] = 0;
        (*x)->sign = 0;
    }

    // dst 사이즈 할당 다시 생각해보자.
    if(*dst == NULL){
        if(bi_new(dst, (*x)->word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if(*dst != NULL && (*dst)->word_len < (*x)->word_len){
        if(bi_resize(dst, (*x)->word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }

    bigint** t = NULL; // t0, t1을 할당
    bigint* temp = NULL;
    msg result_msg = BI_EXP_MS_FAIL;
    byte bit = 0;

    // timeattack을 방지하고자 배열로 선언
    t = (bigint**)calloc(sizeof(bigint*), 2);
    if(t == NULL)    return BI_ALLOC_FAIL;

    // bit 연산 수행할 건데 refine이 되어 있지 않으면 0bit에 대한 쓰레기 연산이 있을 수 있기에 수행
    result_msg = bi_refine(x);
    if(result_msg != BI_SET_REFINE_SUCCESS)    goto EXIT_EXP_T;
    // t0 = 1
    result_msg = bi_new(&t[0], 1);
    t[0]->a[0] = 1;
    if(result_msg != BI_ALLOC_SUCCESS)    goto EXIT_EXP_T;
    // t1 = x (t1 = src)
    result_msg = bi_assign(&t[1], src);
    if(result_msg != BI_SET_ASSIGN_SUCCESS)    goto EXIT_EXP_T0;

    for(int i = (*x)->word_len * WORD_BITS - 1; i >= 0; i--){
        // 상위 비트부터 가져오기
        bit = ((*x)->a[i / WORD_BITS] >> (i % WORD_BITS)) & 1;
        // t[1-bit] = t[0] * t[1] mod n (mod 연산은 추후)
        result_msg = bi_mul_karachuba(&t[1-bit], &t[0], &t[1]);
        if(result_msg != BI_MUL_SUCCESS)    goto EXIT_EXP;
        result_msg = bi_div(&temp, &t[1-bit], &t[1-bit], n);
        if(result_msg != BI_DIV_SUCCESS)    goto EXIT_EXP;

        // t[bit] = t[bit] * t[bit] mod n (mod 연산은 추후)
        result_msg = bi_squ(&t[bit], &t[bit]);
        if(result_msg != BI_SQU_SUCCESS)    goto EXIT_EXP;
        result_msg = bi_div(&temp, &t[bit], &t[bit], n);
        if(result_msg != BI_DIV_SUCCESS)    goto EXIT_EXP;
    }

    if(bi_assign(dst, &t[0]) != BI_SET_ASSIGN_SUCCESS)    goto EXIT_EXP;
    result_msg = BI_EXP_MS_SUCCESS;

EXIT_EXP:
    if(bi_delete(&t[1]) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
EXIT_EXP_T0:
    if(bi_delete(&t[0]) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
EXIT_EXP_T:
    free(t);
    return result_msg;
}