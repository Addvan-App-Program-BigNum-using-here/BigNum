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
    int word_size_a = (*a)->word_len;
    int word_size_b = (*b)->word_len;

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
        carry = (temp_a > MAX_VALUE - (temp_b + carry) || temp_b > MAX_VALUE - (temp_a + carry) || (temp_a == MAX_VALUE && temp_b == MAX_VALUE)) ? 1 : 0; // carry bit 계산
    }
    if (carry)  (*dst)->a[max_word_len] = carry;
    (*dst)->sign = 0;

    if(bi_resize(dst, dst_word_len - !carry) != BI_RESIZE_SUCCESS){ // carry ^ 1을 한 이유는 마지막 carry가 발생하지 않으면 사이즈를 1 word 줄여도 되서
        if(bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
        return BI_RESIZE_FAIL;
    }

    if(*dst != *a && bi_resize(a, word_size_a) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    if(*dst != *b && bi_resize(b, word_size_b) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;

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
    int word_size_b = (*b)->word_len;

    // 여기서 첫 barrow 계산할 때 a랑 b를 전체를 넣으면 안되고 a[0] b[0]끼리 비교를 해줘야 한다.
    byte borrow = (*a)->a[0] >= (*b)->a[0] ? 0 : 1; // 여기 첫 borrow 수행할 때 a >= b일 때 0, a < b 일 때 1
    byte borrow_temp = 0; // 이전 word에서 사용한 borrow 값을 뺄 때 사용

    if(*dst == NULL){
        if(bi_new(dst, max_word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if(*dst != NULL && (*dst)->word_len < max_word_len){ // 해당 부분은 나중에 bi_resize 함수 만들어서 최적화 해보자
        if(bi_resize(dst, max_word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }

    if(bi_expand(b, max_word_len, 0) != BI_EXPAND_SUCCESS)    return BI_EXPAND_FAIL;
    // 뺄셈 연산 수행

    for(int i = 0; i < (*a)->word_len - 1; i++){
        // borrow를 사용할 경우 0x100000000을 더해줘야 하는데 이를 0xffffffff + 0x1로 나누어 더해 준다. 그리고 이전에 빌려갔던 borrow_temp를 빼준다.
        (*dst)->a[i] = (word)(borrow * MAX_VALUE - (*b)->a[i] + (*a)->a[i] + borrow - borrow_temp);
        borrow_temp = borrow;
        borrow = ((*a)->a[i + 1] < (*b)->a[i + 1] + borrow || ((*b)->a[i + 1] == MAX_VALUE && borrow)) ? 1 : 0; // borrow bit 계산
    }
    (*dst)->a[max_word_len - 1] = (word)(borrow * MAX_VALUE - (*b)->a[max_word_len - 1] + (*a)->a[max_word_len - 1] + borrow - borrow_temp);

    // 부호 설정
    (*dst)->sign = 0;
    if(bi_resize(dst, max_word_len) != BI_RESIZE_SUCCESS){
        if(bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
        return BI_RESIZE_FAIL;
    }

    if(*dst!= *b && bi_resize(b, word_size_b) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
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

    for (int i = (temp_len_b * 2) - 1; i >= 0; i--){
        b_i = (i % 2 == 0) ? GET_LOWER_PART((*b)->a[i / 2]) : GET_UPPER_PART((*b)->a[i / 2]);

        idx_temp1 = 0;
        idx_temp2 = 0;
        // temp1, temp2 초기화
        for(int j = 0; j < temp1->word_len; j++)    temp1->a[j] = 0;
        for(int j = 0; j < temp2->word_len; j++)    temp2->a[j] = 0;

        // 곱셈 수행
        for (int j = 0; j < temp_len_a * 2; j++) { // word_len == 2
            if (j % 2 == 0) temp1->a[idx_temp1++] = (word)(b_i * GET_LOWER_PART((*a)->a[j / 2]));
            else    temp2->a[idx_temp2++] = (word)(b_i * GET_UPPER_PART((*a)->a[j / 2]));
        }

        result_msg = bi_shift_left(&temp1, &temp1, i * WORD_SHIFT); // 16비트 단위로 밀기
        if (result_msg != BI_SHIFT_SUCCESS)
            goto MUL_EXIT;

        result_msg = bi_shift_left(&temp2, &temp2, (i + 1) * WORD_SHIFT); // 16비트 단위로 밀기
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

    if (bi_assign(dst, &dst_temp) != BI_SET_ASSIGN_SUCCESS)    goto MUL_EXIT;
    if (bi_resize(dst, max_word_len) != BI_RESIZE_SUCCESS)    goto MUL_EXIT;
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
// 카라츄바의 조건상 두 입력 값의 크기는 동일한 크기이다.
msg bi_mul_karachuba(OUT bigint **dst, IN bigint **a, IN bigint **b, IN int karachuba_flag){
    if (*a == NULL || *b == NULL) return MEM_NOT_ALLOC;
    msg result_msg = BI_MUL_FAIL;

    // 입력 값이 동일한 조건으로 인해 아래 함수는 필요 없음 -> 입력 값이 다를 경우 필요
//    if(bi_refine(a) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;
//    if(bi_refine(b) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;

//    int word_len = (*a)->word_len; // 입력 값이 같기에 하나만 가져오면 된다.

    int len_a = (*a)->word_len;
    int len_b = (*b)->word_len;
    int max_word_len = max(len_a, len_b); // 입력 값 중 최댓값 가져오기 -> 입력 값 크기가 다를 경우 필요
    int min_word_len = min(len_a, len_b); // 입력 값 중 최솟값 가져오기 -> 입력 값 크기가 다를 경우 필요
    int dst_word_len = len_a + len_b;

    // base case에서 카라츄바가 아닌 일반 곱셈 수행을 위한 연산 -> 입력 값 크기가 다를 경우 word_len을 max_word_len으로 변경
    if(karachuba_flag >= min_word_len || karachuba_flag <= 1){
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
//    int half_word_len = (word_len + 1) >> 1; // 입력 값 중 최댓값의 절반 값        -> 입력 값 크기가 같은 경우

    int half_word_len = (max_word_len + 1) >> 1; // 입력 값 중 최댓값의 절반 값  -> 입력 값 크기가 다를 경우
    // a와 b의 사이즈가 다른 경우 사이즈를 동일하게 맞춰줘야 함 -> 이것도 입력 값 크기가 다를 경우에만 사용
    if((*a)->word_len != max_word_len){ // a < b
        if(bi_resize(a, max_word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }else if((*b)->word_len != max_word_len){ // a > b
        if(bi_resize(b, max_word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }

    a_sign = (*a)->sign;
    b_sign = (*b)->sign;
    (*a)->sign = 0;
    (*b)->sign = 0;

    // A_1, B_1 계산
    result_msg = bi_shift_right(&a_1, a, half_word_len * WORD_BITS);
    if (result_msg != BI_SHIFT_SUCCESS) goto karachuba_exit;

    result_msg = bi_shift_right(&b_1, b, half_word_len * WORD_BITS);
    if (result_msg != BI_SHIFT_SUCCESS) goto karachuba_exit;

    // A_0, B_0 계산
    result_msg = bi_get_lower(&a_0, a, half_word_len * WORD_BITS);
    if (result_msg != BI_GET_LOWER_SUCCESS) goto karachuba_exit;

    result_msg = bi_get_lower(&b_0, b, half_word_len * WORD_BITS);
    if (result_msg != BI_GET_LOWER_SUCCESS) goto karachuba_exit;

    result_msg = bi_mul_karachuba(&a_0b_0, &a_0, &b_0, karachuba_flag);
    if(result_msg != BI_MUL_SUCCESS)    goto karachuba_exit;

    result_msg = bi_mul_karachuba(&a_1b_1, &a_1, &b_1, karachuba_flag);
    if (result_msg != BI_MUL_SUCCESS)   goto karachuba_exit;

    // (A_1 * B_1) || (A_0 * B_0) => dst
    result_msg = bi_cat(dst, &a_1b_1, &a_0b_0);
    if (result_msg != BI_CAT_SUCCESS)   goto karachuba_exit;

    // (A_1 * B_1) + (A_0 * B_0) - (A_1 - A_0) * (B_1 - B_0)
    result_msg = bi_sub(&a_1_a_0, &a_1, &a_0); // A_1 - A_0 => a_1_a_0
    if(result_msg != BI_SUB_SUCCESS)    goto karachuba_exit;

    result_msg = bi_sub(&b_1_b_0, &b_1, &b_0); // B_1 - B_0 => b_1_b_0
    if (result_msg != BI_SUB_SUCCESS)   goto karachuba_exit;

    result_msg = bi_add(&a_1b_1, &a_1b_1, &a_0b_0); // (A_1 * B_1) + (A_0 * B_0) => a_1b_1
    if (result_msg != BI_ADD_SUCCESS)   goto karachuba_exit;

    // (A_1 - A_0) * (B_1 - B_0) => a_1_a_0 // 분할 정복
    result_msg = bi_mul_karachuba(&a_1_a_0, &a_1_a_0, &b_1_b_0, karachuba_flag);
    if (result_msg != BI_MUL_SUCCESS)   goto karachuba_exit;

    // (A_1 * B_1) + (A_0 * B_0) - (A_1 - A_0) * (B_1 - B_0) => a_1b_1
    result_msg = bi_sub(&a_1b_1, &a_1b_1, &a_1_a_0);
    if (result_msg != BI_SUB_SUCCESS)   goto karachuba_exit;

    // ((A_1 * B_1) + (A_0 * B_0) - (A_1 - A_0) * (B_1 - B_0)) << half_word_len * WORD_BITS => a_1b_1
    result_msg = bi_shift_left(&a_1b_1, &a_1b_1, half_word_len * WORD_BITS);
    if (result_msg != BI_SHIFT_SUCCESS) goto karachuba_exit;

    // (A_1 * B_1) + ((A_1 * B_1) + (A_0 * B_0) - (A_1 - A_0) * (B_1 - B_0)) + (A_0 * B_0)
    result_msg = bi_add(dst, dst, &a_1b_1);
    if (result_msg != BI_ADD_SUCCESS)   goto karachuba_exit;

    // 부호 처리
    (*dst)->sign = a_sign ^ b_sign; // XOR 연산으로 부호 처리 다르면 음수, 같으면 양수
    if(*dst != *a)  (*a)->sign = a_sign;
    if(*dst != *b)  (*b)->sign = b_sign;

    result_msg = bi_resize(dst, dst_word_len);
    if(result_msg != BI_RESIZE_SUCCESS)    goto karachuba_exit;
    if(*dst != *a)  result_msg = bi_resize(a, len_a);
    if(result_msg != BI_RESIZE_SUCCESS)    goto karachuba_exit;
    if(*dst != *b)  result_msg = bi_resize(b, len_b);
    if(result_msg != BI_RESIZE_SUCCESS)    goto karachuba_exit;

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
msg init_karachuba_pool(IN int max_word_size){
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
msg bi_div(OUT bigint **q, OUT bigint **r, IN bigint **a, IN bigint **b, IN int option){
    if(*a == NULL || *b == NULL)    return MEM_NOT_ALLOC;
    // 몫과 나머지가 같은 메모리를 가리키면 안됨
    if(*q != NULL && *q == *r)    return DIV_FAIL;

    msg result_msg = BI_DIV_FAIL;
    bigint* temp_r = NULL;

    // b가 0이거나 할당이 안되었을 경우
    result_msg = bi_is_zero(b);
    if(result_msg == BI_NOT_USING || result_msg == BI_IS_ZERO){
        return BI_DIV_BY_ZERO;
    }

    if((*b)->sign){ // b가 음수인 경우
        if((*a)->sign){ // a, b 모두 음수인 경우
            (*a)->sign = 0;
            (*b)->sign = 0;
            result_msg = bi_div(q, r, a, b, option);
            if(result_msg != BI_DIV_SUCCESS)    return result_msg;
            if(*a != *q && *a != *r)    (*a)->sign = 1;
            if(*b != *q && *b != *r)    (*b)->sign = 1;
            (*r)->sign = 1;
            result_msg = BI_DIV_SUCCESS;
            goto EXIT_DIV_REFINE;
        }
        // a가 양수인 경우
        (*b)->sign = 0; // b는 양수로
        (*a)->sign = 1; // a는 음수로
        result_msg = bi_div(q, r, a, b, option);
        if(result_msg != BI_DIV_SUCCESS)    return result_msg;
        if(*a != *r && *a != *q)    (*a)->sign = 0;
        if(*b != *r && *b != *q)    (*b)->sign = 1;
        result_msg = BI_DIV_SUCCESS;
        goto EXIT_DIV_REFINE;
    }else if((*a)->sign){ // a가 음수, b가 양수인 경우
        // 1 만들기
        bigint* one = NULL;
        bigint* temp_r = NULL;
        if(bi_new(&one, 1) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
        one->a[0] = 1;

        (*a)->sign = 0;
        result_msg = bi_div(q, &temp_r, a, b, option);
        if(result_msg != BI_DIV_SUCCESS)    return result_msg;
        if(*a != temp_r && *a != *q)    (*a)->sign = 1;

        // q와 r 처리
        if(bi_sub(r, b, &temp_r) != BI_SUB_SUCCESS)    return BI_SUB_FAIL; // r = b - r 수행. 여기서 나눗셈 연산 과정에서 b가 변하면 안된다!

        (*q)->sign = 0; // -q
        if(bi_add(q, q, &one) != BI_ADD_SUCCESS)    return BI_ADD_FAIL; // q = -q - 1 수행 // 일단 여기서 문제 발생
        (*q)->sign = 1; // q = -q - 1
        result_msg = BI_DIV_SUCCESS;
        goto EXIT_DIV_REFINE;
    }

    // 해당 코드로 짤 경우 time attack 가능성이 있음
    int bi_compare = bi_compare_abs(a, b);
    if(bi_compare <= 0){ // a가 b보다 작거나 같은 경우
        if(*q == NULL){
            if(bi_new(q, 1) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
        }else{
            if(bi_resize(q, 1) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
        }
        (*q)->word_len = 1;
        if(!bi_compare){ // a == b인 경우
            (*q)->a[0] = 1;
            (*q)->sign = ((*a)->sign ^ (*b)->sign) ? 1 : 0; // 부호가 다르면 음수, 같으면 양수
            if(*r == NULL){
                if(bi_new(r, 1) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
            }else{
                if(bi_resize(r, 1) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
            }
            (*r)->word_len = 1;
            (*r)->a[0] = 0;
        }else{ // case a < b
            (*q)->a[0] = (word)(*a)->sign;
            (*q)->sign = (*a)->sign;
            // r 메모리 할당
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

    // WORD LONG DIVISION
    if(option == WORD_LONG_DIV){
        // general division k value 찾기
        int k = 0;
        word temp = (*b)->a[(*b)->word_len - 1];
        while(temp){
            temp >>= 1;
            k++;
        }
        k = WORD_BITS - k;
        result_msg = divc_gener(&temp_q, &temp_r, a, b, k);
    }else{ // BINARY DIVISION
        result_msg = divc(&temp_q, &temp_r, a, b);
    }
    if(result_msg != DIVC_SUCCESS)  goto EXIT_DIV;

    result_msg = bi_assign(q, &temp_q);
    if(result_msg != BI_SET_ASSIGN_SUCCESS)    goto EXIT_DIV;

    // r이 NULL이면 할당
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

/*************************************************
 * Name:        divc_gener
 *
 * Description: Bigint division using word division
 *
 * Arguments:   - bigint** q: pointer to bigint struct
 *              - bigint** r: pointer to bigint struct
 *              - bigint** a: bigint struct
 *              - bigint** b: bigint struct
 **************************************************/
// 0 < B < A < BW 조건에서 보통 수행한다.
msg divc_gener(OUT bigint** q, OUT bigint** r, IN bigint** a, IN bigint** b, IN int k){
    if(*a == NULL || *b == NULL)    return MEM_NOT_ALLOC;
    msg result_msg = DIVC_FAIL;

    int init_shift_size = (*a)->word_len - (*b)->word_len + 1;
    bigint* R = NULL;
    bigint* a_p = NULL;
    bigint* b_p = NULL;
    word q_p = 0;
    bigint* r_p = NULL;
  
    // b와 동일한 사이즈 만큼 가져오기 -> b와 계속 비교하면서 수행할 것이에
    result_msg = bi_shift_right(&R, a, init_shift_size * WORD_BITS);
    if(result_msg != BI_SHIFT_SUCCESS)    return DIVC_FAIL;
  
    for(int i = init_shift_size - 1; i >= 0; i--){
        result_msg = bi_shift_left(&R, &R, WORD_BITS); // r_p = r_p << WORD_BITS
        if(result_msg != BI_SHIFT_SUCCESS)    goto EXIT_DIVC_GENER;
        R->a[0] = (*a)->a[i]; // R = R | a[i]
        if(bi_compare_abs(&R, b) >= 0){ // 여기가 psuedo code에서 DIVC
            result_msg = bi_shift_left(&a_p, &R, k); // a_p = a << k
            if(result_msg != BI_SHIFT_SUCCESS)    goto EXIT_DIVC_GENER;
            result_msg = bi_shift_left(&b_p, b, k); // b_p = b << k
            if(result_msg != BI_SHIFT_SUCCESS)    goto EXIT_DIVC_GENER;
            result_msg = bi_refine(&a_p);
            if(result_msg != BI_SET_REFINE_SUCCESS)    goto EXIT_DIVC_GENER;
            result_msg = bi_refine(&b_p);
            if(result_msg != BI_SET_REFINE_SUCCESS)    goto EXIT_DIVC_GENER;

            result_msg = divcc(&q_p, &r_p, &a_p, &b_p); // R, a_p, b_p를 이용한 나눗셈 수행
            if(result_msg != DIVCC_SUCCESS)    goto EXIT_DIVC_GENER;
            result_msg = bi_shift_right(&R, &r_p, k); // r_p = r_p >> k
            if(result_msg != BI_SHIFT_SUCCESS)    goto EXIT_DIVC_GENER;
        }
        (*q)->a[i] = q_p;
    }
    result_msg = bi_assign(r, &R);
    if(result_msg != BI_SET_ASSIGN_SUCCESS)    goto EXIT_DIVC_GENER;

    result_msg = DIVC_SUCCESS;

EXIT_DIVC_GENER:
    if(bi_delete(&R) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&a_p) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&b_p) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&r_p) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    return result_msg;
}

/*************************************************
 * Name:        divcc
 *
 * Description: Bigint division using word division
 *
 * Arguments:   - bigint** q: pointer to bigint struct
 *              - bigint** r: pointer to bigint struct
 *              - bigint** a: bigint struct
 *              - bigint** b: bigint struct
 **************************************************/
msg divcc(OUT word* q, OUT bigint** r, IN bigint** a, IN bigint** b){
    if(*a == NULL || *b == NULL)    return MEM_NOT_ALLOC;
    msg result_msg = DIVCC_FAIL;
    int n = (*a)->word_len;
    int m = (*b)->word_len;
    bigint* temp_a = NULL;
    bigint* temp_b = NULL;
    int power_decom[WORD_BITS] = {0, };
    int decom_size = 0;

    if(n == m)
        *q = (word)((*a)->a[m - 1] / (*b)->a[m - 1]); // q = a[n] / b[n]
    if(n == m + 1){
        if((*a)->a[m] == (*b)->a[m - 1])
            (*q) = (1ULL << WORD_BITS) - 1;
        else{
            result_msg = bi_new(&temp_a, 2);
            if(result_msg != BI_ALLOC_SUCCESS)    goto EXIT_DIVCC;
            temp_a->a[1] = (*a)->a[m];
            temp_a->a[0] = (*a)->a[m - 1];
            result_msg = two_word_long_div(q, &temp_a, (*b)->a[m - 1]);
            if(result_msg != TWO_WORD_LONG_DIV_SUCCESS)    goto EXIT_DIVCC;
        }
    }
    result_msg = bi_new(&temp_a, (*b)->word_len);
    if(result_msg != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    // B * Q
    decom_size = get_power_decomposition(*q, power_decom); // Q에 대한 power decomposition
    result_msg = bi_assign(&temp_b, b);
    if(result_msg != BI_SET_ASSIGN_SUCCESS)    goto EXIT_DIVCC;
    if(power_decom[0]){
        result_msg = bi_assign(&temp_a, &temp_b);
        if(result_msg != BI_SET_ASSIGN_SUCCESS)    goto EXIT_DIVCC;
    }
    for(int i = 1; i < decom_size; i++){
        result_msg = bi_shift_left(&temp_b, &temp_b, 1);
        bi_refine(&temp_b);
        if(result_msg != BI_SHIFT_SUCCESS)    goto EXIT_DIVCC;
        if(power_decom[i] == 0) continue;
        result_msg = bi_add(&temp_a, &temp_b, &temp_a); // 여기서 덧셈 문제 발생
    }

    // A - B * Q
    result_msg = bi_sub(r, a, &temp_a);
    if(result_msg != BI_SUB_SUCCESS)    goto EXIT_DIVCC;

    while((*r)->sign){
        (*q)--;
        result_msg = bi_add(r, r, b);
        if(result_msg != BI_ADD_SUCCESS)    goto EXIT_DIVCC;
    }

    result_msg = DIVCC_SUCCESS;
EXIT_DIVCC:
    if(bi_delete(&temp_a) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&temp_b) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    return result_msg;
}

msg two_word_long_div(OUT word* q, IN bigint** a, IN word b){
    if(*a == NULL)    return MEM_NOT_ALLOC;
    if(b == 0)    return BI_DIV_BY_ZERO;
    if((*a)->word_len > 2)    return TWO_WORD_LONG_DIV_LENGTH_INVALID;

    (*q) = 0;
    word r_temp = (*a)->a[1]; // R <- A_1
    for(int i = WORD_BITS - 1; i >= 0; i--){
        if(r_temp >= (1ULL << (WORD_BITS - 1))){
            (*q) |= 1ULL << i;
            r_temp = (r_temp << 1) + (((*a)->a[0] >> i) & 1) - b; // R <- 2R + a_j - B
        }else{
            r_temp = (r_temp << 1) + (((*a)->a[0] >> i) & 1); // R <- 2R + a_j
            if(r_temp >= b){
                (*q) |= 1ULL << i;
                r_temp -= b;
            }
        }
    }

    return TWO_WORD_LONG_DIV_SUCCESS;
}

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
        if(j % 2 == 0)  temp = GET_LOWER_PART((*a)->a[j / 2]);
        else if(j % 2 == 1)    temp = GET_UPPER_PART((*a)->a[j / 2]);
        C1 -> a[j] = temp * temp;
        for(int i = j + 1; i < max_word_len; i++){
            if(i % 2 == 1)  T2 = temp * GET_UPPER_PART((*a)->a[i / 2]); // 위에서 연산했던 temp 재사용
            else if(i % 2 == 0) T2 = temp * GET_LOWER_PART((*a)->a[i / 2]);
            if((i + j) % 2 == 0)    C3 ->a[(i + j) / 2] = T2; // 짝수
            else C2 -> a[(i + j) / 2] = T2; // 홀수
        }
        result_msg = bi_shift_left(&C2, &C2, WORD_SHIFT); // 16비트 단위로 밀기
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

    result_msg = bi_resize(dst, max_word_len);
    if(result_msg != BI_RESIZE_SUCCESS) goto EXIT_SQU;

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
msg bi_squ_karachuba(OUT bigint** dst, IN bigint** a, IN int squ_karachuba_flag){
    if (*a == NULL) return MEM_NOT_ALLOC;
    msg result_msg = 0;

    // base case에서 카라츄바가 아닌 일반 제곱 수행을 위한 연산
    if(squ_karachuba_flag >= (*a)->word_len || squ_karachuba_flag <= 1){
        result_msg = bi_squ(dst, a);
        if(result_msg != BI_SQU_SUCCESS)    return result_msg;
        return BI_SQU_SUCCESS;
    }

    bigint* a_0 = g_pool.pool[g_pool.current_depth][0];
    bigint* a_1 = g_pool.pool[g_pool.current_depth][1];
    bigint* a_1a_1 = g_pool.pool[g_pool.current_depth][2];
    bigint* a_0a_0 = g_pool.pool[g_pool.current_depth][3];
    g_pool.current_depth++;

    (*a)->sign = 0;

    int half_word_len = (((*a)->word_len) + 1) >> 1; // 길이의 절반 가져오기
    int loop_len = half_word_len - ((*a)->word_len % 2); // 반복 횟수

    if(bi_resize(&a_1, half_word_len) != BI_RESIZE_SUCCESS)    return BI_SHIFT_FAIL;
    if(bi_resize(&a_0, half_word_len) != BI_RESIZE_SUCCESS)    return BI_SHIFT_FAIL;

    // a_1, a_0 나누기
    for(int i= 0; i < loop_len; i++){
        a_1->a[i] = (*a)->a[i + half_word_len];
        a_0->a[i] = (*a)->a[i];
    }
    if((*a)->word_len % 2 == 1) a_0->a[loop_len] = (*a)->a[loop_len]; // 길이가 홀수인 경우 반으로 나눌 때

    result_msg = bi_squ_karachuba(&a_1a_1, &a_1, squ_karachuba_flag);
    if(result_msg != BI_SQU_SUCCESS)    goto karachuba_exit;

    result_msg = bi_squ_karachuba(&a_0a_0, &a_0, squ_karachuba_flag);
    if (result_msg != BI_SQU_SUCCESS)   goto karachuba_exit;

    // (A_1 * A_1) || (A_0 * A_0) => dst
    result_msg = bi_cat(dst, &a_1a_1, &a_0a_0);
    if (result_msg != BI_CAT_SUCCESS)   goto karachuba_exit;

    // A_1 * A_0  분할 정복
    result_msg = bi_mul_karachuba(&a_0a_0, &a_1, &a_0, a_1->word_len / mul_karachuba_ratio);
    if (result_msg != BI_MUL_SUCCESS)   goto karachuba_exit;

    // ((A_1 * A_0) << half_word_len * WORD_BITS + 1 => 2 * a_1a_0
    result_msg = bi_shift_left(&a_0a_0, &a_0a_0, half_word_len * WORD_BITS + 1);
    if (result_msg != BI_SHIFT_SUCCESS) goto karachuba_exit;

    // (A_1 * A_1) + 2 * A_1 * A_0 + (A_0 * A_0)
    result_msg = bi_add(dst, dst, &a_0a_0);
    if (result_msg != BI_ADD_SUCCESS)   goto karachuba_exit;

    // 부호 처리
    (*dst)->sign = 0; // 제곱 연산이므로 항상 양수

    result_msg = BI_SQU_SUCCESS;

karachuba_exit:
    g_pool.current_depth--;
    return result_msg;
}

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
    // src가 음수인 경우도 사실 부호 처리만 하면 되니까 상관 없기는 한데 일단 test에서 전처리를 수행함
    // 음수인 경우 -> 이거는 역원 찾는거라서 일단 패스 -> 이거 test에서 전처리 함
    if((*x)->sign)    return BI_EXP_MS_FAIL;
    // 0인 경우 time attack을 방지하고자 0값 할당해서 수행하자.
    if(bi_is_zero(x) == BI_IS_ZERO){
        (*x)->word_len = 1;
        (*x)->a[0] = 0;
        (*x)->sign = 0;
    }

    bigint** t = NULL; // t0, t1을 할당
    bigint* temp = NULL;
    msg result_msg = BI_EXP_MS_FAIL;
    byte bit = 0;
    int div_option = 1;

    // timeattack을 방지하고자 배열로 선언
    t = (bigint**)calloc(2, sizeof(bigint*));
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
        result_msg = bi_mul_karachuba(&t[1-bit], &t[0], &t[1], t[1]->word_len / mul_karachuba_ratio);
        if(result_msg != BI_MUL_SUCCESS)    goto EXIT_EXP;
        result_msg = bi_refine(&t[1-bit]);
        if(result_msg != BI_SET_REFINE_SUCCESS)    goto EXIT_EXP;
        result_msg = bi_div(&temp, &t[1-bit], &t[1-bit], n, div_option);
        if(result_msg != BI_DIV_SUCCESS)    goto EXIT_EXP;

        result_msg = bi_refine(&t[1-bit]);
        if(result_msg != BI_SET_REFINE_SUCCESS)    goto EXIT_EXP;

        // t[bit] = t[bit] * t[bit] mod n (mod 연산은 추후)
        result_msg = bi_squ(&t[bit], &t[bit]);
        if(result_msg != BI_SQU_SUCCESS)    goto EXIT_EXP;
        result_msg = bi_refine(&t[bit]);
        if(result_msg != BI_SET_REFINE_SUCCESS)    goto EXIT_EXP;
        result_msg = bi_div(&temp, &t[bit], &t[bit], n, div_option);
        if(result_msg != BI_DIV_SUCCESS)    goto EXIT_EXP;
        result_msg = bi_refine(&t[bit]);
        if(result_msg != BI_SET_REFINE_SUCCESS)    goto EXIT_EXP;
    }


    result_msg = bi_assign(dst, &t[0]);
    if(result_msg != BI_SET_ASSIGN_SUCCESS)    goto EXIT_EXP;

    result_msg = BI_EXP_MS_SUCCESS;

EXIT_EXP:
    if(bi_delete(&t[1]) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
EXIT_EXP_T0:
    if(bi_delete(&t[0]) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
EXIT_EXP_T:
    free(t);
    return result_msg;
}

msg bi_exp_R_TO_L(OUT bigint** dst, IN bigint** src, IN bigint** x, IN bigint** n){
    if(*src == NULL || *n == NULL)    return MEM_NOT_ALLOC;
    // src가 음수인 경우도 사실 부호 처리만 하면 되니까 상관 없기는 한데 일단 test에서 전처리를 수행함
    // 음수인 경우 -> 이거는 역원 찾는거라서 일단 패스 -> 이거 test에서 전처리 함
    if((*x)->sign)    return BI_EXP_R_TO_L_FAIL;
    // 0인 경우 time attack을 방지하고자 0값 할당해서 수행하자.
    if(bi_is_zero(x) == BI_IS_ZERO){
        (*x)->word_len = 1;
        (*x)->a[0] = 0;
        (*x)->sign = 0;
    }

    bigint* t0 = NULL;
    bigint* t1 = NULL;
    bigint* temp = NULL;
    msg result_msg = BI_EXP_R_TO_L_FAIL;
    byte bit = 0;
    int div_option = 1;

    // t0 = 1
    result_msg = bi_new(&t0, 1);
    t0->a[0] = 1;

    // t1 = src 초기 값 세팅
    result_msg = bi_assign(&t1, src);
    if(result_msg != BI_SET_ASSIGN_SUCCESS)    goto EXIT_EXP;

    for(int i = 0; i < (*x)->word_len * WORD_BITS; i++){
        bit = ((*x)->a[i / WORD_BITS] >> (i % WORD_BITS)) & 1; // 하위 비트 가져오기
        // bit가 1일 경우에만 곱셈 수행
        if(bit){
            result_msg = bi_mul_karachuba(&t0, &t0, &t1, t1->word_len / mul_karachuba_ratio);
            if(result_msg != BI_MUL_SUCCESS)    goto EXIT_EXP;
            bi_refine(&t0);
            result_msg = bi_div(&temp, &t0, &t0, n, div_option);
            if(result_msg != BI_DIV_SUCCESS)    goto EXIT_EXP;
            bi_refine(&t0);
        }
        result_msg = bi_squ(&t1, &t1);
        if(result_msg != BI_SQU_SUCCESS)    goto EXIT_EXP;
        bi_refine(&t1);
        result_msg = bi_div(&temp, &t1, &t1, n, div_option);
        if(result_msg != BI_DIV_SUCCESS)    goto EXIT_EXP;
        bi_refine(&t1);
    }

    result_msg = bi_assign(dst, &t0);
    if(result_msg != BI_SET_ASSIGN_SUCCESS)    goto EXIT_EXP;

    result_msg = BI_EXP_R_TO_L_SUCCESS;

EXIT_EXP:
    if(bi_delete(&t0) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&t1) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&temp) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    return result_msg;
}


msg bi_exp_L_TO_R(OUT bigint** dst, IN bigint** src, IN bigint** x, IN bigint** n){
    if(*src == NULL || *n == NULL)    return MEM_NOT_ALLOC;
    // src가 음수인 경우도 사실 부호 처리만 하면 되니까 상관 없기는 한데 일단 test에서 전처리를 수행함
    // 음수인 경우 -> 이거는 역원 찾는거라서 일단 패스 -> 이거 test에서 전처리 함
    if((*x)->sign)    return BI_EXP_L_TO_R_FAIL;
    // 0인 경우 time attack을 방지하고자 0값 할당해서 수행하자.
    if(bi_is_zero(x) == BI_IS_ZERO){
        (*x)->word_len = 1;
        (*x)->a[0] = 0;
        (*x)->sign = 0;
    }

    msg result_msg = BI_EXP_L_TO_R_FAIL;
    byte bit = 0;
    bigint* temp = NULL;
    int div_option = 1;

    // bit 연산 수행할 건데 refine이 되어 있지 않으면 0bit에 대한 쓰레기 연산이 있을 수 있기에 수행
    result_msg = bi_refine(x);
    if(result_msg != BI_SET_REFINE_SUCCESS)    return result_msg;

    // t = 1
    if(*dst == NULL){
        if(bi_new(dst, 1) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if((*dst)->word_len != 1){
        if(bi_resize(dst, 1) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }
    (*dst)->a[0] = 1;

    for(int i = (*x)->word_len * WORD_BITS - 1; i >= 0; i--){
        // 상위 비트부터 가져오기
        bit = ((*x)->a[i / WORD_BITS] >> (i % WORD_BITS)) & 1;

        // t 제곱 수행 ( t <- t^2 ) 제곱 카라츄바의 경우 아직 최적화가 안되어 있기 때문에 일단 제곱 수행
//        result_msg = bi_squ_karachuba(dst, dst, (*dst)->word_len / squ_karachuba_ratio); // 카라츄바 사용
        result_msg = bi_squ(dst, dst); // non 카라츄바
        if(result_msg != BI_SQU_SUCCESS)    goto EXIT_EXP;
        bi_refine(dst);
        result_msg = bi_div(&temp, dst, dst, n, div_option);
        if(result_msg != BI_DIV_SUCCESS)    goto EXIT_EXP;
        bi_refine(dst);
       // bit가 1일 경우에만 곱셈 수행 (t <- t * src)
        if(bit){
            result_msg = bi_mul_karachuba(dst, dst, src, (*src)->word_len / mul_karachuba_ratio);
            if(result_msg != BI_MUL_SUCCESS)    goto EXIT_EXP;
            bi_refine(dst);
            result_msg = bi_div(&temp, dst, dst, n, div_option); // 몫은 필요없으니까 일단 temp에 저장
            if(result_msg != BI_DIV_SUCCESS)    goto EXIT_EXP;
            bi_refine(dst);
        }
    }
    result_msg = BI_EXP_L_TO_R_SUCCESS;

EXIT_EXP:
    return result_msg;
}

msg barret_reduction(OUT IN bigint** dst, IN bigint** a, IN bigint** n, IN bigint** n_barret){
    if(*a == NULL || *n == NULL)    return MEM_NOT_ALLOC;

    msg result_msg = BI_BARRET_REDUCTION_FAIL;

    if((*a)->sign){ // a가 음수인 경우
        (*a)->sign = 0;
        result_msg = barret_reduction(dst, a, n, n_barret);
        if(result_msg != BI_BARRET_REDUCTION_SUCCESS)    return result_msg;

        (*a)->sign = 1;
        result_msg = bi_sub(dst, n, dst);
        if(result_msg != BI_SUB_SUCCESS)    return result_msg;
        return BI_BARRET_REDUCTION_SUCCESS;
    }

    bigint* q_hat = NULL;

    // floor(A / W^(k-1)) 계산
    result_msg = bi_shift_right(&q_hat, a, ((*n)->word_len) * WORD_BITS);
    if(result_msg != BI_SHIFT_SUCCESS)    return result_msg;

    // q_hat * n_barret 계산
    result_msg = bi_mul_karachuba(&q_hat, &q_hat, n_barret, (*n_barret)->word_len / mul_karachuba_ratio);
    if(result_msg != BI_MUL_SUCCESS)    return result_msg;
    bi_refine(&q_hat);

    // floor(Q_hat / W^(k+1)) 계산
    result_msg = bi_shift_right(&q_hat, &q_hat, ((*n)->word_len + 2) * WORD_BITS);
    if(result_msg != BI_SHIFT_SUCCESS)    return result_msg;

    // A - Q_hat * n 계산
    result_msg = bi_mul_karachuba(&q_hat, &q_hat, n, (*n)->word_len / mul_karachuba_ratio);
    if(result_msg != BI_MUL_SUCCESS)    return result_msg;
    bi_refine(&q_hat);

    result_msg = bi_sub(dst, a, &q_hat);
    if(result_msg != BI_SUB_SUCCESS)    return result_msg;

    while(bi_compare_abs(dst, n) >= 0){
        result_msg = bi_sub(dst, dst, n);
        if(result_msg != BI_SUB_SUCCESS)    return result_msg;
    }

    return BI_BARRET_REDUCTION_SUCCESS;
}

msg init_barret_N(OUT bigint** barret_t, IN bigint** barret_n, IN int barret_word_len){
    msg result_msg = INIT_BARRET_N_FAIL;
    bigint* temp = NULL;
    bigint* one = NULL;
    int div_option = 1;

    // W^(2k) 생성
    result_msg = bi_new(&one, (2 * barret_word_len + 2) + 1);
    one->a[(2 * barret_word_len + 2)] = 1;
    if(result_msg != BI_ALLOC_SUCCESS)    return result_msg;

    // barret_n 생성
    result_msg = bi_get_random(barret_n, barret_word_len);
    if (result_msg != BI_GET_RANDOM_SUCCESS)    return result_msg;
    else if((*barret_n)->word_len != barret_word_len){
        result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
        return result_msg;
    }
    (*barret_n)->sign = 0;

    // barret_t 계산
    result_msg = bi_div(barret_t, &temp, &one, barret_n, div_option);
    if(result_msg != BI_DIV_SUCCESS)    return result_msg;

    return INIT_BARRET_N_SUCCESS;
}

msg bi_gcd(OUT bigint** dst, IN bigint** a, IN bigint** b){
    // a,b의 부호와 상관없이 gcd 값은 항상 gcd(|a|, |b|)와 같아서 부호 신경 x
    bigint* t1 = NULL;
    bigint* t2 = NULL;
    bigint* temp = NULL;
    msg result_msg = BI_GCD_FAIL;
    int div_option = 1;

    // a = 0이면 gcd(a,b) = b
    if(bi_is_zero(a) == BI_IS_ZERO){
        result_msg = bi_assign(dst,b);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        return BI_GCD_SUCCESS;
    }
    // b = 0이면 gcd(a,b) = a
    else if(bi_is_zero(b) == BI_IS_ZERO){
        result_msg = bi_assign(dst,a);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        return BI_GCD_SUCCESS;
    }

    // (dst, t1) <- (a, b)
    result_msg = bi_assign(dst, a);
    if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
    result_msg = bi_assign(&t1, b);
    if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;

    // t1 = 0이 될 때까지 반복문 수행
    while(bi_is_zero(&t1) != BI_IS_ZERO){
        // t2 <- dst
        result_msg = bi_assign(&t2, dst);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) goto EXIT_EUC;
        // dst <- t1
        result_msg = bi_assign(dst, &t1);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) goto EXIT_EUC;
        // t1 <- t2 mod t1
        result_msg = bi_div(&temp, &t1, &t2, &t1, div_option); // 몫은 필요없어서 일단 temp에 저장
        if(result_msg != BI_DIV_SUCCESS) goto EXIT_EUC;
    }
    // gcd(a, b) = gcd(|a|, |b|) (a,b 부호 상관 x)
    (*dst)->sign = 0;
    result_msg = bi_refine(dst);
    if(result_msg != BI_SET_REFINE_SUCCESS) return result_msg;

    result_msg = BI_GCD_SUCCESS;

EXIT_EUC:
    if(bi_delete(&t1) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&t2) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    return result_msg;
}

msg bi_EEA(OUT bigint** gcd, OUT bigint** x, OUT bigint** y, IN bigint** a, IN bigint** b){
    // a,b 부호 확인 필요
    // x,y 유일하게 하려면 어떻게 할지 생각해봐야됨 gcd(a,b) = ax + by
    msg result_msg = BI_EEA_FAIL;

//    printf("A : ");
//    bi_print(a, 16);
//    printf("B : ");
//    bi_print(b, 16);

    (*a)->sign = 0;
    (*b)->sign = 0;
    
    // a와 b 둘 다 0이면 gcd(a,b) = 0
    if((bi_is_zero(a) == BI_IS_ZERO) && (bi_is_zero(b) == BI_IS_ZERO)){
        result_msg = bi_new(gcd, 1);
        if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
        result_msg = bi_new(x, 1);
        if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
        result_msg = bi_new(y, 1);
        if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
        return BI_EEA_SUCCESS;
    }
    // a = 0이면 gcd(a,b) = b,
    // x = 0, y = 1 (ax + by= b)
    else if(bi_is_zero(a) == BI_IS_ZERO){
        result_msg = bi_assign(gcd,b);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        result_msg = bi_new(x, 1);
        if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
        result_msg = bi_new(y, 1);
        if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
        (*y)->a[0] = 1;
        return BI_EEA_SUCCESS;
    }
    // b = 0이면 gcd(a,b) = a
    // x = 1, y = 0 (ax + by = a)
    else if(bi_is_zero(b) == BI_IS_ZERO){
        result_msg = bi_assign(gcd,a);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        result_msg = bi_new(y, 1);
        if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
        result_msg = bi_new(x, 1);
        if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
        (*x)->a[0] = 1;
        return BI_EEA_SUCCESS;
    }

    // a < b면 a > b가 되도록 함수 다시 호출, 어차피 a, b 양수
    if(bi_compare(a,b) == -1) bi_EEA(gcd, x, y, b, a);

    //t0 == gcd, u0 == x, v0 == y
    bigint* t1 = NULL;
    bigint* u1 = NULL;
    bigint* u2 = NULL;
    bigint* v1 = NULL;
    bigint* v2 = NULL;
    bigint* q = NULL;
    bigint* r = NULL;
    bigint* temp = NULL;
    int div_option = 1; // WORD LONG DIV 사용

    // (t0, t1) <- (a, b)
    result_msg = bi_assign(gcd, a);
    if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
    result_msg = bi_assign(&t1, b);
    if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
//    printf("step 1\n");
    // (u0, v0) <- (1, 0)
    result_msg = bi_new(x, 1);
    if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
    result_msg = bi_new(y, 1);
    if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
    (*x) -> a[0] = 1;

//    printf("step 2\n");
    // (u1, v1) <- (0, 1)
    result_msg = bi_new(&u1, 1);
    if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
    result_msg = bi_new(&v1, 1);
    if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
    v1 -> a[0] = 1;
//    printf("step 3\n");

    // t1 != 0인 동안 반복문 수행
    while(bi_is_zero(&t1) != BI_IS_ZERO){
        //(q, r) <- div(t0, t1)
        printf("gcd : ");
        bi_print(gcd, 16);
        printf("t1 : ");
        bi_print(&t1, 16);
        result_msg = bi_div(&q, &r, gcd, &t1, div_option);
        if(result_msg != BI_DIV_SUCCESS) return result_msg;
        printf("dividion\n");
        printf("Q : ");
        bi_print(&q, 16);
        printf("R : ");
        bi_print(&r, 16);
        printf("step 4\n");
        //(t0, t1) <- (t1, r)
        result_msg = bi_assign(gcd, &t1);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        result_msg = bi_assign(&t1, &r);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        printf("step 5\n");
        //(u2, v2) <- (u0 - q * u1, v0 - q*v1)
        printf("mul\n");
        printf("v1 : ");
        bi_print(&v1, 16);
        result_msg = bi_mul_karachuba(&temp, &q, &u1, q->word_len / mul_karachuba_ratio); // q * v1 / temp는 r로 재사용 가능
        if(result_msg != BI_MUL_SUCCESS) return result_msg;
        bi_refine(&temp);
        printf("sub\n");
        printf("v2 : ");
        bi_print(&v2, 16);
        printf("x : ");
        bi_print(x, 16);
        result_msg = bi_sub(&u2, x, &temp); // u0 - q * u1
        if(result_msg != BI_SUB_SUCCESS) return result_msg;

        printf("mul\n");
        result_msg = bi_mul_karachuba(&temp, &q, &v1, q->word_len / mul_karachuba_ratio); // q * v1 / temp는 q, r로 재사용 가능
        if(result_msg != BI_MUL_SUCCESS) return result_msg;
        bi_refine(&temp);
        printf("result temp : ");
        bi_print(&temp, 16);
        printf("sub\n");
        printf("Y : ");
        bi_print(y, 16);
        result_msg = bi_sub(&v2, y, &temp); // v0 - q * v1
        if(result_msg != BI_SUB_SUCCESS) return result_msg;
        printf("step 6\n");
        //(u0, v0) <- (u1, v1)
        result_msg = bi_assign(x, &u1);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        result_msg = bi_assign(y, &v1);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        printf("step 7\n");
        //(u1, v1) <- (u2, v2)
        result_msg = bi_assign(&u1, &u2);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        result_msg = bi_assign(&v1, &v2);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        printf("step 8\n");
        // u2, v2 초기화 -> 최적화 가능
        for(int i = 0; i < u2-> word_len; i++) u2 -> a[i] = 0;
        for(int i = 0; i < v2-> word_len; i++) v2 -> a[i] = 0;
        printf("step 9\n");
        printf("t1 : ");
        bi_print(&t1, 16);
    }
//    printf("inner end\n");
    result_msg = BI_EEA_SUCCESS;
    // 일단 x,y(u0, v0)가 음수가 돼도 상관없도록 구현함(음수를 허용할지 항상 양수만 나오게 할지는 선택사항)
    return result_msg;
}