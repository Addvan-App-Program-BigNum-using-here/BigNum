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
msg bi_set_from_array(OUT bigint** dst, const IN int word_len, const IN word* data, const IN int endian){
    int endian_idx = endian ? 0 : word_len - 1;
    int idx = endian ? 1 : -1;
    if (*dst == NULL){
        if(bi_new(dst, word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if((*dst != NULL && (*dst)->word_len != word_len)){
        if(bi_resize(dst, word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }

    // sign bit set
    for (int i = 0; i < word_len; i++){
        if (data[endian_idx] > (word)MAX_VALUE){
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
msg bi_set_from_string(OUT bigint **dst, IN char *int_str, IN int base){
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
    switch (base) {
        case 2: // Binary
            word_len = (str_len + (WORD_BITS - 1)) / WORD_BITS;
            block_size = WORD_BITS;
            break;
        case 10: // Decimal
            if (WORD_BITS == 64)
                word_len = (str_len + 18) / 19; // 64비트는 10진수 19자리 표현 가능
            else if (WORD_BITS == 32)
                word_len = (str_len + 9) / 10;  // 32비트는 10자리 표현 가능
            else if (WORD_BITS == 16)
                word_len = (str_len + 4) / 5;   // 16비트는 5자리 표현 가능
            else if (WORD_BITS == 8)
                word_len = (str_len + 2) / 3;   // 8비트는 3자리 표현 가능
            else
                return BI_SET_STRING_FAIL;
            block_size = 0; // 10진수는 블록 크기를 정의하지 않음
            break;
        case 16: // Hexadecimal
            word_len = (str_len + (WORD_BITS / 4 - 1)) / (WORD_BITS / 4);
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
        else if((*dst)->word_len < (*src)->word_len){
            if(bi_resize(dst, (*src)->word_len) != BI_RESIZE_SUCCESS)    return BI_SET_ASSIGN_FAIL;
        }
    }
    (*dst)->sign = (*src)->sign;
    for(int i = 0; i < (*src)->word_len; i++)   (*dst)->a[i] = (*src)->a[i];
    for(int i = (*src)->word_len; i < (*dst)->word_len; i++)    (*dst)->a[i] = 0;
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
msg bi_print(IN bigint **dst, const IN int base) {
    if (*dst == NULL || (*dst)->a == NULL)  return PRINT_NULL;
    if ((*dst)->sign)   printf("-");
    if (base == 16) printf("0x");

    const char *format = NULL;
    switch (WORD_BITS) {
        case 64:
            format = "%016llx";
            break;
        case 32:
            format = "%08x";
            break;
        case 16:
            format = "%04x";
            break;
        case 8:
            format = "%02x";
            break;
        default:
            printf("Unsupported WORD_BITS value: %d\n", WORD_BITS);
            return PRINT_FAIL;
    }

    for (int i = (*dst)->word_len - 1; i >= 0; i--) {
        printf(format, (*dst)->a[i]);
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
        if(bi_assign(dst, src) != BI_SET_ASSIGN_SUCCESS)    return BI_SET_ASSIGN_FAIL;
        return BI_SHIFT_SUCCESS;
    }

    int max_src_len = (*src)->word_len;
    if(bi_refine(src) != BI_SET_REFINE_SUCCESS)    return BI_RESIZE_FAIL;
    int word_len = (*src)->word_len;
    int shift_word = shift_len / WORD_BITS; // word 단위로 시프트
    int shift_bit = shift_len % WORD_BITS; // bit 단위로 시프트
    int new_word_len = word_len + shift_word + (shift_bit > 0); // 새로 할당할 bigint 길이
    max_src_len = max(max_src_len, new_word_len);

    if(*dst == NULL){
        if(bi_new(dst, max_src_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if(*dst != NULL && (*dst)->word_len != max_src_len){ // dst가 이미 할당되어 있을 경우
        if(bi_resize(dst, max_src_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
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
        (*dst)->a[new_word_len - 1] &= MAX_VALUE >> (WORD_BITS - shift_bit); // 메모리 재활용 할 때 사용하지 않은 bit는 0 처리
    }
    (*dst)->a[0] = (*dst)->a[0] << shift_bit; // bit 이동 시 수식으로 인해 0번째는 수행 못하기에 여기서 수행
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
        if(bi_assign(dst, src) != BI_SET_ASSIGN_SUCCESS)    return BI_SET_ASSIGN_FAIL;
        return BI_SHIFT_SUCCESS;
    }

    int word_len = (*src)->word_len;
    int shift_word = shift_len / WORD_BITS; // word 단위로 시프트
    int shift_bit = shift_len % WORD_BITS; // bit 단위로 시프트
    int new_word_len = word_len - shift_word; // 새로 할당할 bigint 길이
    int flag = 0;
    bigint* one = NULL;
    msg result_msg = BI_SHIFT_FAIL;

    // shift 결과 값이 0인 경우
    if(new_word_len <= 0){
        if(bi_new(dst, 1) != BI_ALLOC_SUCCESS)    return BI_SHIFT_FAIL;
        return BI_SHIFT_SUCCESS;
    }

    if(*dst == NULL){
        if(bi_new(dst, new_word_len) != BI_ALLOC_SUCCESS)    return BI_SHIFT_FAIL;
        flag = 1;
    }else if(*dst != NULL && (*dst)->word_len != new_word_len){ // dst가 이미 할당되어 있을 경우
        if(bi_resize(dst, new_word_len) != BI_RESIZE_SUCCESS)    return BI_SHIFT_FAIL;
    }

    // 음수의 경우 2의 보수 처럼 해야 한다. => 여기서는 부호 뺀 값에 1을 더해준다
    if((*src)->sign){
        // 1 생성
        result_msg = bi_new(&one, 1);
        if(result_msg != BI_ALLOC_SUCCESS)  goto EXIT_SHIFT;
        one->a[0] = 1;
        // 1 더하기
        result_msg = bi_add(src, src, &one);
        if(result_msg != BI_ADD_SUCCESS)    goto EXIT_SHIFT;
    }

    // word 이동
    for(int i = shift_word; i < word_len; i++)  (*dst)->a[i - shift_word] = (*src)->a[i];

    // bit 이동
    if (shift_bit != 0){
        for (int i = 0; i < new_word_len - 1; i++){
            (*dst)->a[i] = ((*dst)->a[i] >> shift_bit) | ((*dst)->a[i + 1] << (WORD_BITS - shift_bit));
        }
        (*dst)->a[new_word_len - 1] = (*dst)->a[new_word_len - 1] >> shift_bit; // 마지막 인덱스 처리
    }


    // 음수의 경우 2의 보수 처럼 해야 한다. => 여기서는 시프트 해준 결과 값에 1을 빼준다.
    if((*src)->sign){
        result_msg = bi_sub(dst, dst, &one);
        if(result_msg != BI_SUB_SUCCESS)    goto EXIT_SHIFT;
    }

    // 부호
    (*dst)->sign = (*src)->sign;

    return  BI_SHIFT_SUCCESS;

EXIT_SHIFT:
    if(flag && bi_delete(dst) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&one) != BI_FREE_SUCCESS)    return BI_FREE_FAIL; // 사용한 1 해제
    return result_msg;
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

    if(*dst == NULL){
        if(bi_new(dst, new_word_len) != BI_ALLOC_SUCCESS)    return BI_SHIFT_FAIL;
    }else if(*dst != NULL && (*dst)->word_len != new_word_len){ // dst가 이미 할당되어 있을 경우
        if(bi_resize(dst, new_word_len) != BI_RESIZE_SUCCESS)    return BI_SHIFT_FAIL;
    }

    // word 이동 / dst와 src가 다를 경우
    if(*dst != *src){
        (*dst)->sign = (*src)->sign; // 부호 복사
        for(int i = 0; i < new_word_len; i++)   (*dst)->a[i] = (*src)->a[i];
    }

    // bit 이동
    if(mod_bit != 0 && mod_bit != 31)    (*dst)->a[new_word_len - 1] = (*dst)->a[new_word_len - 1] & ((1 << (mod_bit + 1)) - 1);
    else if(mod_bit == 31)    (*dst)->a[new_word_len - 1] = (*dst)->a[new_word_len - 1] & ((1 << mod_bit) - 1);

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

    if(*dst == NULL){
        if(bi_new(dst, new_word_len) != BI_ALLOC_SUCCESS)    return BI_SHIFT_FAIL;
    }else if(*dst != NULL && (*dst)->word_len != new_word_len){ // dst가 이미 할당되어 있을 경우
        if(bi_resize(dst, new_word_len) != BI_RESIZE_SUCCESS)    return BI_SHIFT_FAIL;
    }
    // 부호 복사
    (*dst)->sign = (*a)->sign;

    if(*dst == *a || *dst == *b){
        if(*dst == *a && *dst != *b){ // dst가 a와 같고 dst가 b와 다를 경우
            if(bi_shift_left(dst, a, (*b)->word_len * WORD_BITS) != BI_SHIFT_SUCCESS)    return BI_SHIFT_FAIL;
            for(int i = 0; i < (*b)->word_len; i++) (*dst)->a[i] = (*b)->a[i]; // b를 dst에 옮기기
            return BI_CAT_SUCCESS;
        }
    }else{ // a, b만 같은 경우 / dst, a, b 모두 다른 경우
        for(int i = 0; i < (*b)->word_len; i++)  (*dst)->a[i] = (*b)->a[i]; // b를 dst에 옮기기
    }

    // 모두 같은 경우 / dst와 b가 같고 a가 다른 경우 / dst, a, b 모두 다른 경우의 a 옮기기
    for(int i = 0; i < (*a)->word_len; i++) (*dst)->a[i + (*b)->word_len] = (*a)->a[i]; // a를 dst에 옮기기

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
* Name:        check_function_run_one_time
*
* Description: get function runtime
*
* Arguments:   - void* func: test function pointer
*              - bigint** dst: pointer to bigint struct result
*              - msg* result_msg: pointer to result_msg
*              - ParamType* param_types : parameter type
*              - int param_count : parameter count
* Return:      - msg : message. SUCESS or FAIL
**************************************************/
double check_function_run_one_time(void* func, bigint** dst, msg* result_msg, ParamType* param_types, int param_count, ...){
    va_list args;
    void* params[MAX_PARAMS];  // 최대 파라미터 개수에 맞게 조정
    va_start(args, param_count);
    clock_t start = 0;
    clock_t end = 0;

    // 가변 인자들을 params 배열에 저장
    for (int i = 0; i < param_count; i++) {
        params[i] = va_arg(args, void*);
    }
    va_end(args);

    // dst를 제외한 파라미터 개수
    switch(param_count) {
        case 1:
            if(param_types[0] == TYPE_INT_PTR){
                msg (*func_1_ptr)(bigint**, int) = (msg (*)(bigint**, int))(func);
                start = clock();
                *result_msg = func_1_ptr(dst, *(int*)params[0]);
                end = clock();
            }else if(param_types[0] == TYPE_BIGINT_PTR){
                msg (*func_1_ptr)(bigint**, bigint**) = (msg (*)(bigint**, bigint**))(func);
                start = clock();
                *result_msg = func_1_ptr(dst, (bigint**)params[0]);
                end = clock();
            }
            break;
        case 2:
            if(param_types[0] == TYPE_BIGINT_PTR && param_types[1] == TYPE_BIGINT_PTR){
                msg (*func_2_ptr)(bigint**, bigint**, bigint**) = (msg (*)(bigint**, bigint**, bigint**))(func);
                start = clock();
                *result_msg = func_2_ptr(dst, (bigint**)params[0], (bigint**)params[1]);
                end = clock();
            }else if(param_types[0] == TYPE_BIGINT_PTR && param_types[1] == TYPE_INT_PTR){
                msg (*func_2_ptr)(bigint**, bigint**, int) = (msg (*)(bigint**, bigint**, int))(func);
                start = clock();
                *result_msg = func_2_ptr(dst, (bigint**)params[0], *(int*)params[1]);
                end = clock();
            }else if(param_types[0] == TYPE_STR_PTR && param_types[1] == TYPE_INT_PTR){
                msg (*func_2_ptr)(bigint**, char*, int) = (msg (*)(bigint**, char*, int))(func);
                start = clock();
                *result_msg = func_2_ptr(dst, (char*)params[0], *(int*)params[1]);
                end = clock();
            }
            break;
        case 3:
            if(param_types[2] == TYPE_BIGINT_PTR){
                msg (*func_3_ptr)(bigint**, bigint**, bigint**, bigint**) = (msg (*)(bigint**, bigint**, bigint**, bigint**))(func);
                start = clock();
                *result_msg = func_3_ptr(dst, (bigint**)params[0], (bigint**)params[1], (bigint**)params[2]);
                end = clock();
            }else if(param_types[0] == TYPE_BIGINT_PTR && param_types[2] == TYPE_INT_PTR){
                msg (*func_3_ptr)(bigint**, bigint**, bigint**, int) = (msg (*)(bigint**, bigint**, bigint**, int))(func);
                start = clock();
                *result_msg = func_3_ptr(dst, (bigint**)params[0], (bigint**)params[1], *(int*)params[2]);
                end = clock();
            }else if(param_types[0] == TYPE_INT_PTR && param_types[1] == TYPE_WORD_ARR_PTR && param_types[2] == TYPE_INT_PTR){
                msg (*func_3_ptr)(bigint**, int, word*, int) = (msg (*)(bigint**, int, word*, int))(func);
                start = clock();
                *result_msg = func_3_ptr(dst, *(int*)params[0], (word*)params[1], *(int*)params[2]);
                end = clock();
            }
            break;
        case 4:
            if(param_types[0] == TYPE_BIGINT_PTR && param_types[1] == TYPE_BIGINT_PTR && param_types[2] == TYPE_BIGINT_PTR && param_types[3] == TYPE_INT_PTR){
                msg (*func_4_ptr)(bigint**, bigint**, bigint**, bigint**, int) = (msg (*)(bigint**, bigint**, bigint**, bigint**, int))(func);
                start = clock();
                *result_msg = func_4_ptr(dst, (bigint**)params[0], (bigint**)params[1], (bigint**)params[2], *(int*)params[3]);
                end = clock();
            }else if(param_types[0] == TYPE_BIGINT_PTR && param_types[1] == TYPE_BIGINT_PTR && param_types[2] == TYPE_BIGINT_PTR && param_types[3] == TYPE_BIGINT_PTR){
                msg (*func_4_ptr)(bigint**, bigint**, bigint**, bigint**, bigint**) = (msg (*)(bigint**, bigint**, bigint**, bigint**, bigint**))(func);
                start = clock();
                *result_msg = func_4_ptr(dst, (bigint**)params[0], (bigint**)params[1], (bigint**)params[2], (bigint**)params[3]);
                end = clock();
            }
            break;
        // 필요에 따라 더 많은 케이스 추가 가능
        default:
            return 0;
    }
    return (double)(end - start) / CLOCKS_PER_SEC;
}

/*************************************************
* Name:        get_power_decomposition
*
* Description: get power decomposition
*
* Arguments:   - word n: word size number
*              - int* powers: pointer to integer array
* Return:      - int : count of power
**************************************************/
int get_power_decomposition(word n, int* powers){
    int count = 0;
    while (n > 0 && count < WORD_BITS) {
        if (n & 1)  powers[count] = 1;
        n >>= 1;
        count++;
    }
    return count;
}

msg miller_rabin_primality(OUT bigint** temp, IN bigint **n, IN int k){
    // n이 NULL이거나 음수
    if (*n == NULL || (*n)->sign == 1)  return MR_FAIL;

    msg result_msg = MR_FAIL;

    if(bi_new(temp, 1) != BI_ALLOC_SUCCESS)    return MR_FAIL;

    // 짝수 or 0, 1 처리
    if(((*n)->a[0] % 2 == 0) || ((*n)->word_len == 1 && (*n)->a[0] <= 1)){
        (*temp)->a[0] = 0;
        return MR_SUCCESS;
    }

    // 필요한 변수
    bigint *n_minus_1 = NULL; // n-1 값 저장
    bigint *n_minus_2 = NULL; // n-2 값 저장
    bigint *q = NULL;         // n-1 = 2^l * q에서 q값
    bigint *a = NULL;         // 랜덤값 a
    bigint *one = NULL; // 상수 1 n-1할 때 쓸거임
    bigint *two = NULL; // 제곱2연산을 위한 변수
    bigint *temp_2 = NULL; // q값 저장

    int l = 0; // l 값 저장

    // n-1을 하기위해서 one 생성
    result_msg = bi_new(&one, 1);
    if (result_msg != BI_ALLOC_SUCCESS)    goto clean;
    one->a[0] = 1;

    // 2 생성
    result_msg = bi_new(&two, 1);
    if (result_msg != BI_ALLOC_SUCCESS)    goto clean;
    two->a[0] = 2;

    // n-1 계산
    if (bi_sub(&n_minus_1, n, &one) != BI_SUB_SUCCESS)  goto clean;
    // n-2 계산
    if (bi_sub(&n_minus_2, &n_minus_1, &one) != BI_SUB_SUCCESS) goto clean;

    // n-1 = 2^l * q 형태로 분해
    result_msg = bi_assign(&q, &n_minus_1);
    if (result_msg != BI_SET_ASSIGN_SUCCESS) goto clean;

    // q가 홀수가 될 때까지 2로 나누기
    while (q->a[0] % 2 == 0){
        if (bi_shift_right(&q, &q, 1) != BI_SHIFT_SUCCESS)  goto clean; // q를 2로 나누기
        l++; // 2로 나눌 때마다 l 증가
    }

    result_msg = bi_refine(&q);
    if (result_msg != BI_SET_REFINE_SUCCESS) goto clean;

    // k번 테스트
    while (k > 0){
        // 과정 3번 : 랜덤한 a 선택 2, n-2 사이의 랜덤한 수
        do{
            result_msg = bi_get_random(&a, (*n)->word_len);
            if (result_msg != BI_GET_RANDOM_SUCCESS) goto clean;
            // a가 2보다 작으면 다시 선택
            if (bi_compare(&a, &two) < 0)   continue;
            // a가 n-2보다 크면 다시 선택
            if (bi_compare(&a, &n_minus_2) > 0) continue;
            break;
        } while (1);
        k--;

        result_msg = bi_gcd(&temp_2, &a, n);
        if (result_msg != BI_GCD_SUCCESS)   goto clean;
        bi_refine(&temp_2);

        // 과정 4번 : gcd(a, n) != 1인 경우 => ~(gcd(a, n) == 1) == ~(word_len == 1 && a[0] == 1) == word_len != 1 || a[0] != 1
        if((temp_2)->word_len != 1 || (temp_2)->a[0] != 1){
            result_msg = bi_new(temp, 1);
            if (result_msg != BI_ALLOC_SUCCESS)    goto clean;
            result_msg = MR_SUCCESS;
            goto clean;
        }

        // step 8 : a <- a^q mod n
        result_msg = bi_exp_L_TO_R(&a, &a, &q, n);
        if (result_msg != BI_EXP_L_TO_R_SUCCESS)  goto clean;

        // step 9 : a가 1이면 다음 테스트로 , step 10
        result_msg = bi_div(&temp_2, &a, &a, n, 1);
        if (result_msg != BI_DIV_SUCCESS)    goto clean;
        result_msg = bi_compare(&a, &one);
        if (result_msg == 0)  continue;
        // step 12 : j = 0 l-1까지

        for (int j = 0; j <= l - 1; j++){
            result_msg = bi_div(&temp_2, &a, &a, n, 1);
            if (result_msg != BI_DIV_SUCCESS)    goto clean;

            result_msg = bi_compare(&a, &n_minus_1);
            if (result_msg == 0)    break;
            // step 11 : a <- a^2 mod n
            result_msg = bi_exp_L_TO_R(&a, &a, &two, n); // 여기 나중에 squ로 하자 (modular squaring 구현 필요)
            if (result_msg != BI_EXP_L_TO_R_SUCCESS)   goto clean;

        }
        if(result_msg == 0) continue;
        result_msg = bi_new(temp, 1);
        if (result_msg != BI_ALLOC_SUCCESS)    goto clean;
        (*temp)->a[0] = 0;
        result_msg = MR_SUCCESS;
        goto clean;
    }
    result_msg = bi_new(temp, 1);
    if (result_msg != BI_ALLOC_SUCCESS)    goto clean;
    (*temp)->a[0] = 1;

    result_msg = MR_SUCCESS;

clean:
    // 메모리 해제
    bi_delete(&n_minus_1);
    bi_delete(&n_minus_2);
    bi_delete(&q);
    bi_delete(&a);
    bi_delete(&one);
    return result_msg;
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
        if(result_msg != BI_SET_ASSIGN_SUCCESS) goto EXIT_GCD;
        // dst <- t1
        result_msg = bi_assign(dst, &t1);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) goto EXIT_GCD;
        // t1 <- t2 mod t1
        result_msg = bi_div(&temp, &t1, &t2, &t1, div_option); // 몫은 필요없어서 일단 temp에 저장
        if(result_msg != BI_DIV_SUCCESS) goto EXIT_GCD;
    }
    // gcd(a, b) = gcd(|a|, |b|) (a,b 부호 상관 x)
    (*dst)->sign = 0;
    result_msg = bi_refine(dst);
    if(result_msg != BI_SET_REFINE_SUCCESS) goto EXIT_GCD;

    result_msg = BI_GCD_SUCCESS;

EXIT_GCD:
    if(bi_delete(&t1) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&t2) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    if(bi_delete(&temp) != BI_FREE_SUCCESS)    return BI_FREE_FAIL;
    return result_msg;
}


msg bi_EEA(OUT bigint** gcd, OUT bigint** x, OUT bigint** y, IN bigint** a, IN bigint** b){
    // a,b 부호 확인 필요
    // x,y 유일하게 하려면 어떻게 할지 생각해봐야됨 gcd(a,b) = ax + by
    msg result_msg = BI_EEA_FAIL;

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

    // (u0, v0) <- (1, 0)
    result_msg = bi_new(x, 1);
    if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
    result_msg = bi_new(y, 1);
    if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
    (*x) -> a[0] = 1;

    // (u1, v1) <- (0, 1)
    result_msg = bi_new(&u1, 1);
    if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
    result_msg = bi_new(&v1, 1);
    if(result_msg != BI_ALLOC_SUCCESS) return result_msg;
    v1 -> a[0] = 1;

    // t1 != 0인 동안 반복문 수행
    while(bi_is_zero(&t1) != BI_IS_ZERO){
        //(q, r) <- div(t0, t1)
        result_msg = bi_div(&q, &r, gcd, &t1, div_option);
        if(result_msg != BI_DIV_SUCCESS) return result_msg;
        //(t0, t1) <- (t1, r)
        result_msg = bi_assign(gcd, &t1);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        bi_refine(gcd);
        result_msg = bi_assign(&t1, &r);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        //(u2, v2) <- (u0 - q * u1, v0 - q*v1)
        result_msg = bi_mul_karachuba(&temp, &q, &u1, q->word_len / mul_karachuba_ratio); // q * v1 / temp는 r로 재사용 가능
        if(result_msg != BI_MUL_SUCCESS) return result_msg;
        bi_refine(&temp);
        result_msg = bi_sub(&u2, x, &temp); // u0 - q * u1
        if(result_msg != BI_SUB_SUCCESS) return result_msg;

        result_msg = bi_mul_karachuba(&temp, &q, &v1, q->word_len / mul_karachuba_ratio); // q * v1 / temp는 q, r로 재사용 가능
        if(result_msg != BI_MUL_SUCCESS) return result_msg;
        bi_refine(&temp);
        result_msg = bi_sub(&v2, y, &temp); // v0 - q * v1
        if(result_msg != BI_SUB_SUCCESS) return result_msg;
        //(u0, v0) <- (u1, v1)
        result_msg = bi_assign(x, &u1);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        result_msg = bi_assign(y, &v1);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        //(u1, v1) <- (u2, v2)
        result_msg = bi_assign(&u1, &u2);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        result_msg = bi_assign(&v1, &v2);
        if(result_msg != BI_SET_ASSIGN_SUCCESS) return result_msg;
        // u2, v2 초기화 -> 최적화 가능
        for(int i = 0; i < u2-> word_len; i++) u2 -> a[i] = 0;
        for(int i = 0; i < v2-> word_len; i++) v2 -> a[i] = 0;
    }
    result_msg = BI_EEA_SUCCESS;
    // 일단 x,y(u0, v0)가 음수가 돼도 상관없도록 구현함(음수를 허용할지 항상 양수만 나오게 할지는 선택사항)
    return result_msg;
}