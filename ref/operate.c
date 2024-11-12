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
msg bi_add(OUT bigint** dst, IN bigint** a, IN bigint** b, IN byte option){
    byte carry = 0;
    msg result_msg;
    int max_word_len = 0;

    if(a == NULL || b == NULL)    return MEM_NOT_ALLOC;
    if(dst != NULL && option == CLEAR){ // NULL이 아니고 CLEAR 옵션인 경우
        if(bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    }

    if((*a)->sign == 0 && (*b)->sign == 1){ // a - b
        (*b)->sign = 0;
        if(bi_sub(dst, a, b, option) != BI_SUB_SUCCESS)    return BI_ADD_FAIL; // a - b
        return BI_ADD_SUCCESS;
    }
    if((*a)->sign == 1 && (*b)->sign == 0){ // -a + b = b - a
        (*a)->sign = 0;
        if(bi_sub(dst, b, a, option) != BI_SUB_SUCCESS)    return BI_SUB_FAIL; // b - a
        return BI_ADD_SUCCESS;
    }
    if((*a)->sign == 1 && (*b)->sign == 1){ // -a + -b = -(a + b)
        (*a)->sign = 0;
        (*b)->sign = 0;
        if(bi_add(dst, a, b, option) != BI_ADD_SUCCESS)    return BI_ADD_FAIL; // a + b
        (*dst)->sign = 1; // -(a + b)
        return BI_ADD_SUCCESS;
    }

    // 상수 시간 연산을 위해 a와 b의 크기를 맞춰서 연산 수행
    if ((*a)->word_len >= (*b)->word_len){
        max_word_len = (*a)->word_len;
        result_msg = bi_expand(b, max_word_len, 0); // 길이 맞추기
        if(result_msg != BI_EXPAND_SUCCESS) return BI_EXPAND_FAIL;
    } else{
        max_word_len = (*b)->word_len;
        result_msg = bi_expand(a, max_word_len, 0); // 길이 맞추기
        if(result_msg != BI_EXPAND_SUCCESS) return BI_EXPAND_FAIL;
    }

    if(option == CLEAR){
        if(bi_new(dst, max_word_len + 1) != BI_ALLOC_SUCCESS){
            if(bi_refine(*a) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;
            if(bi_refine(*b) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;
            return BI_ALLOC_FAIL;
        }
    }
    // 덧셈 연산 수행
    for(int i = 0; i < max_word_len; i++){
        (*dst)->a[i] = (word)((*a)->a[i] + (*b)->a[i] + carry);
        carry = ((*a)->a[i] > 0xffffffff - ((*b)->a[i] + carry) || (*b)->a[i] > 0xffffffff - ((*a)->a[i] + carry) || ((*a)->a[i] == 0xffffffff && (*b)->a[i] == 0xffffffff)) ? 1 : 0; // carry bit 계산
    }
    if(carry)   (*dst)->a[max_word_len] = carry;

    if(bi_refine(*dst) != BI_SET_REFINE_SUCCESS){
        if(option == CLEAR && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
        return BI_SET_REFINE_FAIL;
    }
    if(bi_refine(*a) != BI_SET_REFINE_SUCCESS){
        if(option == CLEAR && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
        return BI_SET_REFINE_FAIL;
    }
    if(bi_refine(*b) != BI_SET_REFINE_SUCCESS){
        if(option == CLEAR && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
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
msg bi_sub(OUT bigint** dst, IN bigint** a, IN bigint** b, IN byte option){
    byte barrow = bi_compare_abs(a, b) >= 0 ? 0 : 1; // 여기 첫 barrow 수행할 때 a >= b일 때 0, a < b 일 때 1
    byte flag = 0;

    if(a == NULL || b == NULL)    return MEM_NOT_ALLOC;
    if(dst != NULL && option == CLEAR){
        if(bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    }

    if((*a)->sign == 0 && (*b)->sign == 1){ // a - (-b) = a + b
        (*b)->sign = 0;
        if(bi_add(dst, a, b, option) != BI_ADD_SUCCESS)    return BI_SUB_FAIL;
        return BI_SUB_SUCCESS;
    }else if((*a)->sign == 1 && (*b)->sign == 0){ // -a - b = -(a + b)
        (*a)->sign = 0;
        if(bi_add(dst, a, b, option) != BI_ADD_SUCCESS)    return BI_SUB_FAIL;
        (*dst)->sign = 1;
        return BI_SUB_SUCCESS;
    }else if((*a)->sign == 1 && (*b)->sign == 1){ // -a - (-b) = b - a
        (*a)->sign = 0;
        (*b)->sign = 0;
        if(bi_sub(dst, b, a, option) != BI_SUB_SUCCESS)    return BI_SUB_FAIL;
        return BI_SUB_SUCCESS;
    }

    // b가 a보다 더 큰 경우
    if(bi_compare_abs(a, b) == -1){
        // b - a
        if(bi_sub(dst, b, a, option) != BI_SUB_SUCCESS)    return BI_SUB_FAIL;
        (*dst)->sign = 1; // -(b - a) // 모듈러 체크
        return BI_SUB_SUCCESS;
    }

    // 상수 시간 연산을 위해 a와 b의 크기를 맞춰서 연산 수행 -> 무조건 a >= b
    if(bi_expand(b, (*a)->word_len, 0) != BI_EXPAND_SUCCESS){ // tmp의 길이를 a와 같게 만듦
        return BI_EXPAND_FAIL;
    }

    if(option == CLEAR){
        if(bi_new(dst, (*a)->word_len) != BI_ALLOC_SUCCESS) return BI_ALLOC_FAIL;
        flag = 1;
    }

    // 뺄셈 연산 수행
    for(int i = 0; i < (*a)->word_len; i++){
        (*dst)->a[i] = (word)(barrow * 0xFFFFFFFF - (*b)->a[i] + (*a)->a[i]);
        barrow = (((*a)->a[i] < (*b)->a[i] + barrow) || ((*b)->a[i] == 0xffffffff && barrow)) ? 1 : 0; // barrow bit 계산
    }

    if(bi_refine(*b) != BI_SET_REFINE_SUCCESS){
        if(flag && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
        return BI_SET_REFINE_FAIL;
    }
    if(bi_refine(*dst) != BI_SET_REFINE_SUCCESS){
        if(flag && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
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
msg bi_mul(OUT bigint **dst, IN bigint **a, IN bigint **b){
    // 데이터 임시 변수들 선언
    bigint *temp1 = NULL;
    bigint *temp2 = NULL;
    bigint *temp3 = NULL;
    bigint *temp4 = NULL;
    // 여기 temp 줄일 수 있는 방법 생각해보자.
    msg result_msg;
    word b_i;
    int temp_len = 0, idx_temp1, idx_temp2;
    byte a_sign = 0, b_sign = 0;

    // 에러 케이스 정의
    if (a == NULL || b == NULL) return MEM_NOT_ALLOC;
    if (dst != NULL){
        if (bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
    }
    if(bi_compare_abs(a, b) == -1){ // a < b
        result_msg = bi_mul(dst, b, a);
        if(result_msg != BI_MUL_SUCCESS)    return BI_MUL_FAIL;
        return BI_MUL_SUCCESS;
    }

    // 최대 길이 : n + m
    int max_len = (*a)->word_len + (*b)->word_len;
    if(bi_new(dst, max_len) != BI_ALLOC_SUCCESS)   return BI_ALLOC_FAIL;
    if(bi_new(&temp4, max_len) != BI_ALLOC_SUCCESS)   return BI_ALLOC_FAIL;

    temp_len = ((*a)->word_len * 2) / 2; // 박스 개수

    a_sign = (*a)->sign;
    b_sign = (*b)->sign;
    (*a)->sign = 0;
    (*b)->sign = 0;

    // 반복문 밖에가 작은 값, 안쪽이 큰 값
    for (int i = ((*b)->word_len * 2) - 1; i >= 0; i--){ // word_len == 2
        result_msg = bi_new(&(temp1), temp_len);
        if(result_msg != BI_ALLOC_SUCCESS)  goto MUL_EXIT;
        result_msg = bi_new(&(temp2), temp_len);
        if(result_msg != BI_ALLOC_SUCCESS)  goto MUL_EXIT;

        b_i = (i % 2 == 0) ? (*b)->a[i / 2] & 0xFFFF : (*b)->a[i / 2] >> 16; // 16비트 단위로 나누어 곱셈 수행
        idx_temp1 = 0;
        idx_temp2 = 0;
        for (int j = 0; j < (*a)->word_len * 2; j++){ // word_len == 2
            if(j % 2 == 0)
                (*temp1).a[idx_temp1++] = (word)(b_i * ((*a)->a[j / 2] & 0xFFFF)); // 하위 16비트 곱
            else
                (*temp2).a[idx_temp2++] = (word)(b_i * ((*a)->a[j / 2] >> 16)); // 상위 16비트 곱
        }

        result_msg = bi_shift_left(&temp1, i * 16); // 16비트 단위로 밀기
        if(result_msg != BI_SHIFT_SUCCESS)  goto MUL_EXIT;
        result_msg = bi_shift_left(&temp2, (i + 1) * 16); // 16비트 단위로 밀기
        if(result_msg != BI_SHIFT_SUCCESS)  goto MUL_EXIT;

        result_msg = bi_add(&temp3, &temp1, &temp2, CLEAR); // 덧셈 수행
        if(result_msg != BI_ADD_SUCCESS)  goto MUL_EXIT;
        result_msg = bi_add(&temp4, dst, &temp3, CLEAR); // 덧셈 수행
        if(result_msg != BI_ADD_SUCCESS)  goto MUL_EXIT;

        result_msg = bi_assign(dst, &temp4); // 결과 저장
        if(result_msg != BI_SET_ASSIGN_SUCCESS)  goto MUL_EXIT;

        if(bi_delete(&temp3) != BI_FREE_SUCCESS)  goto MUL_EXIT;
        if(bi_delete(&temp2) != BI_FREE_SUCCESS)  goto MUL_EXIT;
        if(bi_delete(&temp1) != BI_FREE_SUCCESS)  goto MUL_EXIT;
    }

    // 부호 처리
    (*dst)->sign = a_sign ^ b_sign; // XOR 연산으로 부호 처리 다르면 음수, 같으면 양수
    (*a)->sign = a_sign;
    (*b)->sign = b_sign;

    if (bi_refine(*dst) != BI_SET_REFINE_SUCCESS)   goto MUL_EXIT;
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