#include "util.h"

/*************************************************
* Name:        bi_new
*
* Description: New allocate memory for bigint struct
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - int word_len: length of bigint struct
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg bi_new(OUT bigint** dst, const IN int word_len){
    // dst에 값이 들어가 있는데 word_len이 다를 경우 -> 삭제하고 다시 할당
    if(*dst != NULL){ // dst에 값이 할당되어 있는 경우
        if(word_len != (*dst)->word_len){ // 할당된 메모리가 word_len과 같지 않을 경우 -> 삭제
            if(bi_delete(dst) != BI_FREE_SUCCESS)   return BI_ALLOC_FAIL;
        }else{ // 미리 할당된 데이터가 word_len과 동일할 경우 -> 재사용
            for(int i = 0; i < word_len; i++)
                (*dst)->a[i] = 0;
            (*dst)->sign = 0;
            return BI_ALLOC_SUCCESS;
        }
    }
    // dst가 NULL로 지정되어 있을 경우 -> 할당
    *dst = (bigint*)calloc(1, sizeof(bigint));
    if(*dst == NULL)    return BI_ALLOC_FAIL;

    (*dst)->sign = 0;
    (*dst)->a = (word *)calloc(word_len, sizeof(word));
    if ((*dst)->a == NULL){
        free(*dst);
        *dst = NULL;
        return BI_ALLOC_FAIL;
    }
    (*dst)->word_len = word_len;

    return BI_ALLOC_SUCCESS;
}

/*************************************************
* Name:        bi_delete
*
* Description: Delete allocated memory for bigint struct
*
* Arguments:   - bigint** dst: pointer to bigint struct
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg bi_delete(OUT bigint** dst){
    if(*dst == NULL)    return BI_FREE_SUCCESS;
    if((*dst)->a != NULL)
        free((*dst)->a);
    (*dst)->word_len = 0;
    (*dst)->sign = 0;
    free(*dst);
    *dst = NULL;

    return BI_FREE_SUCCESS;
}

/*************************************************
* Name:        bi_set_from_array
*
* Description: Set bigint struct from array
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - int sign: sign of bigint struct
*              - int word_len: length of bigint struct
*              - word* a: array of word
*              - int endian: little endian == 0, big endian == 1
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg bi_set_from_array(OUT bigint** dst, const IN int sign, const IN int word_len, const IN word* data, const IN int endian){
    int endian_idx = endian ? 0 : word_len - 1;
    int idx = endian ? 1 : -1;
    if (*dst == NULL){
        if(bi_new(dst, word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if((*dst != NULL && (*dst)->word_len != word_len)){
        if(bi_resize(dst, word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }

    // sign bit set
    (*dst)->sign = sign;
    for (int i = 0; i < word_len; i++){
        if (data[endian_idx] > 0xFFFFFFFF)
        {
            printf("DATA_OVERFLOW\n");
            if(bi_delete(dst) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
            return BI_SET_ARRAY_FAIL;
        }
        (*dst)->a[i] = data[endian_idx];
        endian_idx += idx;
    }
    if(bi_refine(dst) != BI_SET_REFINE_SUCCESS) return BI_SET_REFINE_FAIL;
    return BI_SET_ARRAY_SUCCESS;
}

/*************************************************
 * Name:        bi_set_from_string
 *
 * Description: Set bigint struct from string
 *
 * Arguments:   - bigint** dst: pointer to bigint struct
 *              - char* int_str: string of bigint
 *              - int base: base of string (2, 10, 16)
 * Return:      - msg : message. SUCCESS or FAIL
 **************************************************/
msg bi_set_from_string(OUT bigint **dst, IN char *int_str, const IN int base){
    msg result_msg = 0;
    int sign = 0, a_idx = 0, digit = 0, block_size = 0, word_idx = 0, word_len = 0;
    int str_len = strlen(int_str);
    word temp = 0;

    // sign bit check
    if (int_str[0] == '-'){
        sign = 1;
        int_str++;
    }

    // extract word length
    switch (base){
    case 2:
        word_len = (str_len + 31) / 32;
        block_size = WORD_BITS;
        break;
    case 10:
        //            word_len = (str_len + 9) / 10;
        break;
    case 16:
        word_len = (str_len + 7) / 8;
        block_size = WORD_BITS / 4;
        break;
    default:
        printf("%d base is not supported\n", base);
        return BI_SET_STRING_FAIL;
    }

    a_idx = (str_len % block_size == 0) ? block_size : str_len % block_size;
    a_idx += block_size * (word_len - 2); // a_idx 값 뒤의 4byte부터 가져오기
    // memory allocate
    result_msg = bi_new(dst, word_len);
    if (result_msg != BI_ALLOC_SUCCESS){
        log_msg(result_msg);
        return result_msg;
    }
    // sign bit set
    (*dst)->sign = sign;

    // 하위 WORD_BITS만큼씩 끊어서 저장
    while (word_len > 1){
        temp = 0;
        for (int i = 0; i < block_size; i++){
            digit = char_to_int(int_str[a_idx++]);
            temp = temp * base + digit;
        }
        (*dst)->a[word_idx++] = temp;
        word_len--;
        a_idx = a_idx - 2 * block_size;
    }

    // 상위 나머지 부분 저장
    temp = 0;
    a_idx += block_size;
    for (int i = 0; i < a_idx; i++){
        digit = char_to_int(int_str[i]);
        temp = temp * base + digit;
    }
    (*dst)->a[word_idx++] = temp;

    return BI_SET_STRING_SUCCESS;
}

/*************************************************
* Name:        bi_resize
*
* Description: resize bigint struct to data until word length
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - int word_len: length of bigint struct
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg bi_resize(OUT bigint** dst, const IN int word_len){
    msg result_msg;

    // dst가 NULL인 새로 만들기
    if(*dst == NULL){
        if(bi_new(dst, word_len) != BI_ALLOC_SUCCESS)  return BI_ALLOC_FAIL;
        return BI_RESIZE_SUCCESS;
    }

    if((*dst)->word_len > word_len){ // word_len이 기존보다 작게 들어온 경우 맞춰서 할당
        (*dst)->a = (word*)realloc((*dst)->a, word_len * sizeof(word));
        // realloc 실패의 경우
        if((*dst)->a == NULL){
            if(bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
            return BI_ALLOC_FAIL;
        }
        (*dst)->word_len = word_len;
        return BI_RESIZE_SUCCESS;
    }else if((*dst)->word_len < word_len){ // word_len이 기존보다 크게 들어온 경우 expand
        result_msg = bi_expand(dst, word_len, 0);
        if(result_msg != BI_EXPAND_SUCCESS)  return result_msg;
        return BI_RESIZE_SUCCESS;
    }
    return BI_RESIZE_SUCCESS;
}

/*************************************************
* Name:        bi_expand
*
* Description: expand bigint struct to data until word length
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - int word_len: length of bigint struct
*              - word data: data to expand
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg bi_expand(OUT bigint** dst, const IN int word_len, const IN word data){
    byte flag = 0; // 새로 할당 받은 dst인지 여부

    // dst가 NULL인 경우 새로 할당해서 data로 채워주기
    if(*dst == NULL){
        if(bi_new(dst, word_len) != BI_ALLOC_SUCCESS)  return BI_ALLOC_FAIL;
        flag = 1;
    }

    if((*dst)->word_len >= word_len)    return BI_EXPAND_SUCCESS;
    // word_len이 작을 경우 expand가 아니기에 실패 -> 이거는 resize 구현 때 참고하자

    // 동적할당으로 메모리 늘리기
    (*dst)->a = (word *)realloc((*dst)->a, word_len * sizeof(word));
    if ((*dst)->a == NULL){
        if (flag && bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
        return BI_ALLOC_FAIL;
    }

    // 늘린 메모리에 data로 채워주기
    memset((*dst)->a + (*dst)->word_len, data, (word_len - (*dst)->word_len) * sizeof(word)); // memset 수행 확인 필요한가??
    (*dst)->word_len = word_len;

    return BI_EXPAND_SUCCESS;
}

/*************************************************
* Name:        bi_compare
*
* Description: compare bigint struct
*
* Arguments:   - bigint** a: pointer to bigint struct
*              - bigint** b: pointer to bigint struct
* Return:      - int: 1 if a > b, -1 if a < b, 0 if a == b
**************************************************/
int bi_compare(IN bigint** a, IN bigint** b){
    if(*a == NULL || *b == NULL)    return BI_NOT_USING;

    bigint* temp_a = NULL;
    bigint* temp_b = NULL;
    // 원본 값을 복사해서 복사본으로 수행
    if(bi_assign(&temp_a, a) != BI_SET_ASSIGN_SUCCESS)    return BI_SET_ASSIGN_FAIL;
    if(bi_assign(&temp_b, b) != BI_SET_ASSIGN_SUCCESS)    return BI_SET_ASSIGN_FAIL;

    if(bi_refine(&temp_a) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;
    if(bi_refine(&temp_b) != BI_SET_REFINE_SUCCESS)    return BI_SET_REFINE_FAIL;

    // 여기 부호 부터 비교 할 때 -0, 0 이럴 때도 잘 봐야 한다. -> 이거 bi_is_zero로 확인해봐야 할 듯
    if(temp_a->sign == 0 && temp_b->sign == 1) return 1; // a 양수, b 음수
    if(temp_a->sign == 1 && temp_b->sign == 0) return -1; // a 음수, b 양수
    if(temp_a->sign == 1 && temp_b->sign == 1){ // 둘다 음수인 경우
        if(temp_a->word_len > temp_b->word_len) return -1; // a < b
        if(temp_a->word_len < temp_b->word_len) return 1; // a > b
        //길이가 같은 경우
        for(int i = temp_a->word_len - 1 ; i >= 0 ; i--){
            if(temp_a->a[i] > temp_b->a[i]) return -1; // a < b
            if(temp_a->a[i] < temp_b->a[i]) return 1; // a > b
        }
    }
    if(temp_a->sign == 0 && temp_b->sign == 0){ // 둘다 양수인 경우
        if(temp_a->word_len > temp_b->word_len) return 1; // a > b
        if(temp_a->word_len < temp_b->word_len) return -1; // a < b
        //길이가 같은 경우
        for(int i = temp_a->word_len - 1 ; i >= 0 ; i--){
            if(temp_a->a[i] > temp_b->a[i]) return 1; // a > b
            if(temp_a->a[i] < temp_b->a[i]) return -1; // a < b
        }
    }

    return 0;
}

/*************************************************
 * Name:        bi_compare_abs
 *
 * Description: compare absolute bigint struct
 *
 * Arguments:   - bigint** a: pointer to bigint struct
 *              - bigint** b: pointer to bigint struct
 * Return:      - int: 1 if a > b, -1 if a < b, 0 if a == b
 **************************************************/
int bi_compare_abs(IN bigint **a, IN bigint **b){
    if (*a == NULL || *b == NULL)
        return BI_NOT_USING;
    int a_sign = (*a)->sign;
    int b_sign = (*b)->sign;

    (*a)->sign = 0;
    (*b)->sign = 0;
    int result = bi_compare(a, b);
    (*a)->sign = a_sign;
    (*b)->sign = b_sign;

    return result;
}

/*************************************************
* Name:        bi_refine
*
* Description: unnessesary zero value delete, if bi is zero then sign is 0, word_len is 1
*
* Arguments:   - bigint* src: pointer to bigint struct
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg bi_refine(OUT bigint** src){
    if (src == NULL || *src == NULL)    return BI_SET_REFINE_FAIL;

    int new_wordlen = (*src)->word_len;
    while(new_wordlen > 1){
        if((*src)->a[new_wordlen - 1] != 0) break;
        new_wordlen--;
    }
    if ((*src)->word_len != new_wordlen){
        (*src)->word_len = new_wordlen;
        (*src)->a = (word*)realloc((*src)->a, new_wordlen * sizeof(word));
    }

    // refine 후 결과 값이 0인 경우
    if(((*src)->word_len == 1) && ((*src)->a[0] == 0)){
        (*src)->sign = 0;
    }
    return BI_SET_REFINE_SUCCESS;
}

/*************************************************
* Name:        bi_assign
*
* Description: copy bigint struct
*
* Arguments:   - bigint** dst: pointer to bigint struct result
*              - bigint** src: source bigint struct
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg bi_assign(OUT bigint** dst, IN bigint** src){
    if(*src == NULL)   return BI_SET_ASSIGN_FAIL;
    if(*dst == NULL){
        if(bi_new(dst, (*src)->word_len) != BI_ALLOC_SUCCESS)    return BI_SET_ASSIGN_FAIL;
    }else if(*dst != NULL){
        if (*dst == *src)   return BI_SET_ASSIGN_SUCCESS; // 자기 자신에게 할당하는 경우
        else if((*dst)->word_len != (*src)->word_len){
            if(bi_resize(dst, (*src)->word_len) != BI_RESIZE_SUCCESS)    return BI_SET_ASSIGN_FAIL;
        }
    }
    (*dst)->sign = (*src)->sign;
    for(int i = 0; i < (*src)->word_len; i++){
        (*dst)->a[i] = (*src)->a[i];
    }
    return BI_SET_ASSIGN_SUCCESS;
}

/*************************************************
 * Name:        bi_print
 *
 * Description: Print bigint struct
 *
 * Arguments:   - bigint** dst: pointer to bigint struct result
 *              - int base: base of bigint struct (2, 10, 16)
 * Return:      - msg : message. SUCCESS or FAIL
 **************************************************/
msg bi_print(IN bigint **dst, const IN int base){
    if (*dst == NULL || (*dst)->a == NULL)
        return PRINT_NULL;

    if ((*dst)->sign)
        printf("-");
    if (base == 16)
        printf("0x");

    // 간단한 16진수 출력 (10 진수는 이후 추가)
    for (int i = (*dst)->word_len - 1; i >= 0; i--){
        printf("%08x", (*dst)->a[i]);
    }
    printf("\n");

    return PRINT_SUCCESS;
}

/*************************************************
 * Name:        bi_shift_left
 *
 * Description: Shift bigint left
 *
 * Arguments:   - bigint** dst: pointer to bigint struct result
 *              - bigint** src: pointer to bigint struct source
 *              - int shift_len : shift length
 * Return:      - msg : message. SUCCESS or FAIL
 **************************************************/
msg bi_shift_left(OUT bigint **dst, IN bigint **src, const IN int shift_len){
    // shift_len이 word_len보다 클 경우도 생각해야함
    if (*src == NULL)
        return BI_SHIFT_FAIL;

    // shift_len이 0일 경우
    if(shift_len == 0){
        if(*dst == *src)    return BI_SHIFT_SUCCESS;
        if(bi_assign(dst, src) != BI_SET_ASSIGN_SUCCESS)    return BI_SHIFT_FAIL;
        return BI_SHIFT_SUCCESS;
    }

    int max_src_len = (*src)->word_len;
    if(bi_refine(src) != BI_SET_REFINE_SUCCESS)    return BI_SHIFT_FAIL;
    int word_len = (*src)->word_len;
    int shift_word = shift_len / WORD_BITS; // word 단위로 시프트
    int shift_bit = shift_len % WORD_BITS; // bit 단위로 시프트
    int new_word_len = word_len + shift_word + (shift_bit > 0); // 새로 할당할 bigint 길이
    max_src_len = max(max_src_len, new_word_len);

    if(*dst == NULL){
        if(bi_new(dst, max_src_len) != BI_ALLOC_SUCCESS)    return BI_SHIFT_FAIL;
    }else if(*dst != NULL && (*dst)->word_len != max_src_len){ // dst가 이미 할당되어 있을 경우
        if(bi_resize(dst, max_src_len) != BI_RESIZE_SUCCESS)    return BI_SHIFT_FAIL;
    }
    // 부호 복사
    (*dst)->sign = (*src)->sign;

    // word 이동
    if(shift_word != 0){
        for(int i = word_len - 1; i >= 0; i--)  (*dst)->a[i + shift_word] = (*src)->a[i];
        for(int i = 0; i < shift_word; i++) (*dst)->a[i] = 0;
    }else{
        for(int i = 0; i < word_len; i++)   (*dst)->a[i] = (*src)->a[i];
    }

    // bit 이동
    if (shift_bit != 0){
        for (int i = max_src_len - 1; i > 0; i--){
            (*dst)->a[i] = ((*dst)->a[i] << shift_bit) | ((*dst)->a[i - 1] >> (WORD_BITS - shift_bit));
        }
    }
    (*dst)->a[0] = (*dst)->a[0] << shift_bit;
    (*dst)->a[new_word_len - 1] &= 0xffffffff >> (WORD_BITS - shift_bit); // 메모리 재활용 할 때 사용하지 않은 bit는 0 처리
    for(int i = new_word_len; i < max_src_len; i++) (*dst)->a[i] = 0; // 메모리 재활용 할 때 사용하지 않은 word는 0 처리

    return BI_SHIFT_SUCCESS;
}

/*************************************************
 * Name:        bi_shift_right
 *
 * Description: Shift bigint right
 *
 * Arguments:   - bigint** dst: pointer to bigint struct result
 *              - bigint** src: pointer to bigint struct source
 *              - int shift_len : shift length
 * Return:      - msg : message. SUCCESS or FAIL
 **************************************************/
msg bi_shift_right(OUT bigint **dst, IN bigint **src, const IN int shift_len){
    // shift_len이 word_len보다 클 경우도 생각해야함
    if (*src == NULL)   return BI_SHIFT_FAIL;

    // shift_len이 0일 경우
    if(shift_len == 0){
        if(*dst == *src)    return BI_SHIFT_SUCCESS;
        if(bi_assign(dst, src) != BI_SET_ASSIGN_SUCCESS)    return BI_SHIFT_FAIL;
        return BI_SHIFT_SUCCESS;
    }

    int word_len = (*src)->word_len;
    int shift_word = shift_len / WORD_BITS; // word 단위로 시프트
    int shift_bit = shift_len % WORD_BITS; // bit 단위로 시프트
    int new_word_len = word_len - shift_word; // 새로 할당할 bigint 길이
    int flag = 0;
    bigint* one = NULL;

    if(*dst == NULL){
        if(bi_new(dst, new_word_len) != BI_ALLOC_SUCCESS)    return BI_SHIFT_FAIL;
        flag = 1;
    }else if(*dst != NULL && (*dst)->word_len != new_word_len){ // dst가 이미 할당되어 있을 경우
        if(bi_resize(dst, new_word_len) != BI_RESIZE_SUCCESS)    return BI_SHIFT_FAIL;
    }
    // 부호 복사
    (*dst)->sign = (*src)->sign;
    if(bi_assign(dst, src) != BI_SET_ASSIGN_SUCCESS)    return BI_SHIFT_FAIL;

    // 음수의 경우 2의 보수 처럼 해야 한다. => 여기서는 부호 뺀 값에 -1을 해준다.
    if((*dst)->sign){
        // 1 생성
        if(bi_new(&one, 1) != BI_ALLOC_SUCCESS){
            if(flag && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_SHIFT_FAIL;
            return BI_SHIFT_FAIL;
        }
        one->a[0] = 1;
        one->sign = 1;
        if(bi_sub(dst, dst, &one) != BI_SUB_SUCCESS){
            if(flag && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_SHIFT_FAIL;
            if(bi_delete(&one) != BI_FREE_SUCCESS)    return BI_SHIFT_FAIL;
            return BI_SHIFT_FAIL;
        }
    }

    // word 이동
    for(int i = shift_word; i < word_len; i++)  (*dst)->a[i - shift_word] = (*dst)->a[i];

    // bit 이동
    if (shift_bit != 0){
        for (int i = 0; i < new_word_len - 1; i++){
            (*dst)->a[i] = ((*dst)->a[i] >> shift_bit) | ((*dst)->a[i + 1] << (WORD_BITS - shift_bit));
        }
    }
    (*dst)->a[new_word_len - 1] = (*dst)->a[new_word_len - 1] >> shift_bit;

    // 음수의 경우 2의 보수 처럼 해야 한다. => 여기서는 시프트 해준 결과 값에 +1을 한다.
    if((*dst)->sign){
        if(bi_add(dst, dst, &one) != BI_ADD_SUCCESS){
            if(flag && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_SHIFT_FAIL;
            if(bi_delete(&one) != BI_FREE_SUCCESS)    return BI_SHIFT_FAIL;
            return BI_SHIFT_FAIL;
        }
    }

    // 사용한 1 값 해제
    if(bi_delete(&one) != BI_FREE_SUCCESS)    return BI_SHIFT_FAIL;

    // 반환 전에 크기에 맞게 배열 resize
    if(bi_resize(dst, new_word_len) != BI_RESIZE_SUCCESS){
        if(flag && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
        return BI_SHIFT_FAIL;
    }

    return BI_SHIFT_SUCCESS;
}

/*************************************************
 * Name:        bi_get_lower
 *
 * Description: bigint modulation
 *
 * Arguments:   - bigint** dst: pointer to bigint struct result
 *              - bigint** src: pointer to bigint struct source
 *              - int mod_len : length of mod (2^mod_len)
 * Return:      - msg : message. SUCCESS or FAIL
 **************************************************/
msg bi_get_lower(OUT bigint **dst, IN bigint **src, IN int mod_len){
    if (*src == NULL)   return BI_GET_LOWER_FAIL;

    // mod_len이 0일 경우 1에 대한 mod이기 때문에 나머지는 0
    if (mod_len == 0){
        if (bi_new(dst, 1) != BI_ALLOC_SUCCESS) return BI_GET_LOWER_FAIL;
        return BI_GET_LOWER_SUCCESS;
    }
    int mod_word = mod_len / WORD_BITS; // mod_len에 대한 word_len
    int mod_bit = mod_len % WORD_BITS; // bit 단위로 시프트
    int new_word_len = mod_word + (mod_bit > 0); // 새로 할당할 bigint 길이
    int flag = 0;

    if(*dst == NULL){
        if(bi_new(dst, new_word_len) != BI_ALLOC_SUCCESS)    return BI_SHIFT_FAIL;
        flag = 1;
    }else if(*dst != NULL && (*dst)->word_len < new_word_len){ // dst가 이미 할당되어 있을 경우
        if(bi_resize(dst, new_word_len) != BI_RESIZE_SUCCESS)    return BI_SHIFT_FAIL;
    }

    // word 이동 / dst와 src가 다를 경우
    if(*dst != *src){
        (*dst)->sign = (*src)->sign; // 부호 복사
        for(int i = 0; i < new_word_len; i++){
            (*dst)->a[i] = (*src)->a[i];
        }
    }

    // bit 이동
    if(mod_bit != 0 && mod_bit != 31)    (*dst)->a[new_word_len - 1] = (*dst)->a[new_word_len - 1] & ((1 << (mod_bit + 1)) - 1);
    else if(mod_bit == 31)    (*dst)->a[new_word_len - 1] = (*dst)->a[new_word_len - 1] & ((1 << mod_bit) - 1);
    // 반환 전에 크기에 맞게 배열 resize
    if(bi_resize(dst, new_word_len) != BI_RESIZE_SUCCESS){
        if(flag && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
        return BI_SHIFT_FAIL;
    }

    return BI_GET_LOWER_SUCCESS;
}

/*************************************************
* Name:        bi_cat
*
* Description: bigint concatenation
*
* Arguments:   - bigint** dst: pointer to bigint struct result
*              - bigint** a: pointer to bigint struct of concate
*              - bigint** b : pointer to bigint struct of concated
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg bi_cat(OUT bigint** dst, IN bigint** a, IN bigint** b){
    // 두 값 중 하나가 NULL인 경우
    if (*a == NULL || *b == NULL)   return BI_CAT_FAIL;
    // 연접하려는 두 값의 부호가 다를 경우
    if ((*a)->sign != (*b)->sign)   return BI_SIGN_NOT_MATCH;
  
    int new_word_len = (*a)->word_len + (*b)->word_len;
    word* temp = NULL; // dst가 a와 같을 경우 연산에서 사용됨.
    int flag = 0;

    if(*dst == NULL){
        if(bi_new(dst, new_word_len) != BI_ALLOC_SUCCESS)    return BI_SHIFT_FAIL;
        flag = 1;
    }else if(*dst != NULL && (*dst)->word_len != new_word_len){ // dst가 이미 할당되어 있을 경우
        if(bi_resize(dst, new_word_len) != BI_RESIZE_SUCCESS)    return BI_SHIFT_FAIL;
    }
    // 부호 복사
    (*dst)->sign = (*a)->sign;

    // dst가 a와 같고 dst가 b와 다를 경우
    if(*dst == *a && *dst != *b){
       temp = (word*)calloc((*a)->word_len, sizeof(word));
       if(temp == NULL){
          if(flag && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
          return BI_CAT_FAIL;
       }
       for(int i = 0; i < (*a)->word_len; i++)  temp[i] = (*a)->a[i]; // a를 temp에 옮기기
       for(int i = 0; i < (*b)->word_len; i++) (*dst)->a[i] = (*b)->a[i]; // b를 dst에 옮기기
       for(int i = 0; i < (*a)->word_len; i++)  (*dst)->a[i + (*b)->word_len] = temp[i]; // temp를 dst에 옮기기
    }
    // b 값 넣기
    if(*dst != *b){
        for(int i = 0; i < (*b)->word_len; i++) (*dst)->a[i] = (*b)->a[i];
    }
    // a 값 넣기
    for(int i = 0; i < (*a)->word_len; i++) (*dst)->a[i + (*b)->word_len] = (*a)->a[i];

    return BI_CAT_SUCCESS;
}

/*************************************************
* Name:        bi_is_zero
*
* Description: check bigint is zero
*
* Arguments:   - bigint** dst: checked bigint pointer
* Return:      - msg : message. ZERO or NOT_ZERO, NOT_USING
**************************************************/
msg bi_is_zero(bigint **num){
    msg result_msg = BI_IS_ZERO;
    if(*num == NULL)    return BI_NOT_USING;
    // time attack으로 인해 중간에 return 하면 안된다.
    for (int i = 0; i < (*num)->word_len; i++){
        if ((*num)->a[i] != 0)
            result_msg = BI_NOT_ZERO;
    }
    return result_msg; // 모든 요소가 0이면 true 반환
}

/*************************************************
* Name:        check_function_runtime
*
* Description: get function runtime
*
* Arguments:   - msg (*func): test function pointer
*              - bigint** dst: pointer to bigint struct result
*              - bigint** a: pointer to bigint struct of input1
*              - bigint** b : pointer to bigint struct of input2
* Return:      - msg : message. SUCESS or FAIL
**************************************************/
double check_function_run_one_time_two_parm_bigint(IN msg (*func)(OUT bigint**, IN bigint**), OUT bigint** dst, IN bigint** a, IN msg* result_msg){
    clock_t start, end;
    start = clock();
    *result_msg = func(dst, a);
    end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

double check_function_run_one_time_two_parm_int(IN msg (*func)(OUT bigint**, IN int), OUT bigint** dst, IN int n, IN msg* result_msg){
    clock_t start, end;
    start = clock();
    *result_msg = func(dst, n);
    end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

double check_function_run_one_time_three_parm_bigint(IN msg (*func)(OUT bigint**, IN bigint**, IN bigint**), OUT bigint** dst, IN bigint** a, IN bigint** b, IN msg* result_msg){
    clock_t start, end;
    start = clock();
    *result_msg = func(dst, a, b);
    end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

double check_function_run_one_time_three_parm_int(IN msg (*func)(OUT bigint**, IN bigint**, IN int), OUT bigint** dst, IN bigint** a, IN int n, IN msg* result_msg){
    clock_t start, end;
    start = clock();
    *result_msg = func(dst, a, n);
    end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

double check_function_run_one_time_four_parm_bigint(IN msg (*func)(OUT bigint**, IN bigint**, IN bigint**, IN bigint**), OUT bigint** dst, IN bigint** a, IN bigint** b, IN bigint** c, IN msg* result_msg){
    clock_t start, end;
    start = clock();
    *result_msg = func(dst, a, b, c);
    end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}