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
msg bi_new(OUT bigint **dst, const IN int word_len)
{
    if (*dst != NULL)
    {
        if (bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_ALLOC_FAIL;
    }

    *dst = (bigint *)calloc(1, sizeof(bigint));
    if (*dst == NULL)
        return BI_ALLOC_FAIL;

    (*dst)->sign = 0;
    (*dst)->a = (word *)calloc(word_len, sizeof(word));
    if ((*dst)->a == NULL)
    {
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
msg bi_delete(OUT bigint **dst)
{
    if (*dst == NULL)
    {
        return BI_FREE_SUCCESS;
    }

    if ((*dst)->a == NULL)
    {
        return MEM_NOT_ALLOC;
    }

    (*dst)->word_len = 0;
    (*dst)->sign = 0;
    free((*dst)->a);
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
msg bi_set_from_array(OUT bigint **dst, const IN int sign, const IN int word_len, const IN word *data, const IN int endian)
{
    msg result_msg = 0;
    int endian_idx = endian ? 0 : word_len - 1;
    int idx = endian ? 1 : -1;

    result_msg = bi_new(dst, word_len);
    if (result_msg != BI_ALLOC_SUCCESS)
    {
        log_msg(result_msg);
        return result_msg;
    }

    if (dst == NULL)
        return MEM_NOT_ALLOC;

    (*dst)->sign = sign;
    for (int i = 0; i < word_len; i++)
    {
        if (data[endian_idx] > 0xFFFFFFFF)
        {
            printf("DATA_OVERFLOW\n");
            result_msg = bi_delete(dst);
            if (result_msg != BI_FREE_SUCCESS)
                return result_msg;
            return BI_SET_ARRAY_FAIL;
        }
        (*dst)->a[i] = data[endian_idx];
        endian_idx += idx;
    }

    result_msg = bi_refine(*dst);
    if (result_msg != BI_SET_REFINE_SUCCESS)
        return result_msg;

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
msg bi_set_from_string(OUT bigint **dst, IN char *int_str, const IN int base)
{
    msg result_msg = 0;
    int sign = 0, a_idx = 0, digit = 0, block_size = 0, word_idx = 0, word_len = 0;
    int str_len = strlen(int_str);
    word temp = 0;

    // sign bit check
    if (int_str[0] == '-')
    {
        sign = 1;
        int_str++;
    }

    // extract word length
    switch (base)
    {
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
    if (result_msg != BI_ALLOC_SUCCESS)
    {
        log_msg(result_msg);
        return result_msg;
    }
    // sign bit set
    (*dst)->sign = sign;

    // 하위 WORD_BITS만큼씩 끊어서 저장
    while (word_len > 1)
    {
        temp = 0;
        for (int i = 0; i < block_size; i++)
        {
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
    for (int i = 0; i < a_idx; i++)
    {
        digit = char_to_int(int_str[i]);
        temp = temp * base + digit;
    }
    (*dst)->a[word_idx++] = temp;

    return BI_SET_STRING_SUCCESS;
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
msg bi_expand(OUT bigint **dst, const IN int word_len, const IN word data)
{
    msg result_msg;
    byte flag = 0; // 새로 할당 받은 dst인지 여부

    // dst가 NULL인 경우 새로 할당해서 data로 채워주기
    if (*dst == NULL)
    {
        result_msg = bi_new(dst, word_len);
        if (result_msg != BI_ALLOC_SUCCESS)
            return result_msg;
        flag = 1;
    }

    // word_len이 작을 경우 expand가 아니기에 실패
    if ((*dst)->word_len > word_len)
    {
        if (flag && bi_delete(dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
        return BI_EXPAND_FAIL;
    }

    // 동적할당으로 메모리 늘리기
    (*dst)->a = (word *)realloc((*dst)->a, word_len * sizeof(word));
    if ((*dst)->a == NULL)
    {
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
int bi_compare(IN bigint **a, IN bigint **b)
{
    if (*a == NULL || *b == NULL)
        return BI_NOT_USING;

    if (bi_refine(*a) != BI_SET_REFINE_SUCCESS)
        return BI_SET_REFINE_FAIL;
    if (bi_refine(*b) != BI_SET_REFINE_SUCCESS)
        return BI_SET_REFINE_FAIL;

    if ((*a)->sign == 0 && (*b)->sign == 1)
        return 1; // a 양수, b 음수
    if ((*a)->sign == 1 && (*b)->sign == 0)
        return -1; // a 음수, b 양수
    if ((*a)->sign == 1 && (*b)->sign == 1)
    { // 둘다 음수인 경우
        if ((*a)->word_len > (*b)->word_len)
            return -1; // a < b
        if ((*a)->word_len < (*b)->word_len)
            return 1; // a > b
        // 길이가 같은 경우
        for (int i = (*a)->word_len - 1; i >= 0; i--)
        {
            if ((*a)->a[i] > (*b)->a[i])
                return -1; // a < b
            if ((*a)->a[i] < (*b)->a[i])
                return 1; // a > b
        }
    }
    if ((*a)->sign == 0 && (*b)->sign == 0)
    { // 둘다 양수인 경우
        if ((*a)->word_len > (*b)->word_len)
            return 1; // a > b
        if ((*a)->word_len < (*b)->word_len)
            return -1; // a < b
        // 길이가 같은 경우
        for (int i = (*a)->word_len - 1; i >= 0; i--)
        {
            if ((*a)->a[i] > (*b)->a[i])
                return 1; // a > b
            if ((*a)->a[i] < (*b)->a[i])
                return -1; // a < b
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
int bi_compare_abs(IN bigint **a, IN bigint **b)
{
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
msg bi_refine(OUT bigint *src)
{
    if (src == NULL)
    {
        return BI_SET_REFINE_FAIL;
    }

    int new_wordlen = src->word_len;
    while (new_wordlen > 1)
    {
        if (src->a[new_wordlen - 1] != 0)
        {
            break;
        }
        new_wordlen--;
    }
    if (src->word_len != new_wordlen)
    {
        src->word_len = new_wordlen;
        src->a = (word *)realloc(src->a, new_wordlen * sizeof(word));
    }

    if ((src->word_len == 1) && (src->a[0] == 0))
    {
        src->sign = 0;
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
msg bi_assign(OUT bigint **dst, IN bigint **src)
{
    msg result_msg = 0;
    if (*src == NULL)
        return BI_SET_ASSIGN_FAIL;
    if (*dst != NULL)
    {
        if (*dst == *src)
            return BI_SET_ASSIGN_SUCCESS; // 자기 자신에게 할당하는 경우
        else if ((*dst)->word_len != (*src)->word_len)
        {
            if (bi_delete(dst) != BI_FREE_SUCCESS)
                return result_msg;
            if (bi_new(dst, (*src)->word_len) != BI_ALLOC_SUCCESS)
                return BI_SET_ASSIGN_FAIL;
        }
    }

    (*dst)->sign = (*src)->sign;
    memcpy((*dst)->a, (*src)->a, (*src)->word_len * sizeof(word));

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
msg bi_print(IN bigint **dst, const IN int base)
{
    if (*dst == NULL || (*dst)->a == NULL)
        return PRINT_NULL;

    if ((*dst)->sign)
        printf("-");
    if (base == 16)
        printf("0x");

    // 간단한 16진수 출력 (10 진수는 이후 추가)
    for (int i = (*dst)->word_len - 1; i >= 0; i--)
    {
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
msg bi_shift_left(OUT bigint **dst, IN bigint **src, const IN int shift_len)
{
    // shift_len이 word_len보다 클 경우도 생각해야함
    if (*src == NULL)
        return BI_SHIFT_FAIL;

    // shift_len이 0일 경우 복사
    if (shift_len == 0)
    {
        if (bi_assign(dst, src) != BI_SET_ASSIGN_SUCCESS)
            return BI_SHIFT_FAIL;
        return BI_SHIFT_SUCCESS;
    }

    // dst가 비어있지 않을 경우
    if (*dst != NULL)
    {
        // 만약 src와 dst가 동일한 경우 수행한 값을 src에 저장하여 반환
        if (*dst == *src)
        {
            bigint *temp = NULL;
            if (bi_shift_left(&temp, src, shift_len) != BI_SHIFT_SUCCESS)
                return BI_SHIFT_FAIL;
            if (bi_assign(dst, &temp) != BI_SET_ASSIGN_SUCCESS)
                return BI_SHIFT_FAIL;
            if (bi_delete(&temp) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
            return BI_SHIFT_SUCCESS;
        }
        else
        {
            if (bi_delete(dst) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
        }
    }

    int word_len = (*src)->word_len;
    int shift_word = shift_len / WORD_BITS; // word 단위로 시프트
    int shift_bit = shift_len % WORD_BITS;  // bit 단위로 시프트

    // shift_len이 0보다 크니까 무조건 1은 증가
    int new_word_len = word_len + shift_word + (shift_bit > 0); // 새로 할당할 bigint 길이
    if (bi_new(dst, new_word_len) != BI_ALLOC_SUCCESS)
        return BI_SHIFT_FAIL; // 새로운 길이 맞게 할당
    (*dst)->sign = (*src)->sign;

    // word 이동
    for (int i = 0; i < word_len; i++)
    {
        (*dst)->a[i + shift_word] = (*src)->a[i];
    }

    // bit 이동
    if (shift_bit != 0)
    {
        for (int i = new_word_len - 1; i > 0; i--)
        {
            (*dst)->a[i] = ((*dst)->a[i] << shift_bit) | ((*dst)->a[i - 1] >> (WORD_BITS - shift_bit));
        }
    }
    (*dst)->a[0] = (*dst)->a[0] << shift_bit;

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
msg bi_shift_right(OUT bigint **dst, IN bigint **src, const IN int shift_len)
{
    // shift_len이 word_len보다 클 경우도 생각해야함
    if (*src == NULL)
        return BI_SHIFT_FAIL;

    // shift_len이 0일 경우 복사
    if (shift_len == 0)
    {
        if (bi_assign(dst, src) != BI_SET_ASSIGN_SUCCESS)
            return BI_SHIFT_FAIL;
        return BI_SHIFT_SUCCESS;
    }

    // dst가 비어있지 않을 경우
    if (*dst != NULL)
    {
        // 만약 src와 dst가 동일한 경우 수행한 값을 src에 저장하여 반환
        if (*dst == *src)
        {
            bigint *temp = NULL;
            if (bi_shift_right(&temp, src, shift_len) != BI_SHIFT_SUCCESS)
                return BI_SHIFT_FAIL;
            if (bi_assign(dst, &temp) != BI_SET_ASSIGN_SUCCESS)
                return BI_SHIFT_FAIL;
            if (bi_delete(&temp) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
            return BI_SHIFT_SUCCESS;
        }
        else
        {
            if (bi_delete(dst) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
        }
    }

    int word_len = (*src)->word_len;
    int shift_word = shift_len / WORD_BITS; // word 단위로 시프트
    int shift_bit = shift_len % WORD_BITS;  // bit 단위로 시프트

    // shift_len이 0보다 크니까 무조건 1은 감소
    int new_word_len = word_len - shift_word; // 새로 할당할 bigint 길이
    if (bi_new(dst, new_word_len) != BI_ALLOC_SUCCESS)
        return BI_SHIFT_FAIL; // 새로운 길이 맞게 할당
    (*dst)->sign = (*src)->sign;

    // word 이동
    for (int i = shift_word; i < word_len; i++)
    {
        (*dst)->a[i - shift_word] = (*src)->a[i];
    }

    // bit 이동
    if (shift_bit != 0)
    {
        for (int i = 0; i < new_word_len - 1; i++)
        {
            (*dst)->a[i] = ((*dst)->a[i] >> shift_bit) | ((*dst)->a[i + 1] << (WORD_BITS - shift_bit));
        }
    }
    (*dst)->a[new_word_len - 1] = (*dst)->a[new_word_len - 1] >> shift_bit;

    return BI_SHIFT_SUCCESS;
}

/*************************************************
 * Name:        bi_mod
 *
 * Description: bigint modulation
 *
 * Arguments:   - bigint** dst: pointer to bigint struct result
 *              - bigint** src: pointer to bigint struct source
 *              - int mod_len : length of mod (2^mod_len)
 * Return:      - msg : message. SUCCESS or FAIL
 **************************************************/
msg bi_mod(OUT bigint **dst, IN bigint **src, IN int mod_len)
{
    if (*src == NULL)
        return BI_MOD_FAIL;

    // mod_len이 0일 경우 1에 대한 mod이기 때문에 나머지는 0
    if (mod_len == 0)
    {
        if (bi_new(dst, 1) != BI_ALLOC_SUCCESS)
            return BI_MOD_FAIL;
        return BI_MOD_SUCCESS;
    }

    // dst가 비어있지 않을 경우
    if (*dst != NULL)
    {
        // 만약 src와 dst가 동일한 경우 수행한 값을 src에 저장하여 반환
        if (*dst == *src)
        {
            bigint *temp = NULL;
            if (bi_mod(&temp, src, mod_len) != BI_SHIFT_SUCCESS)
                return BI_MOD_FAIL;
            if (bi_assign(dst, &temp) != BI_SET_ASSIGN_SUCCESS)
                return BI_MOD_FAIL;
            if (bi_delete(&temp) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
            return BI_SHIFT_SUCCESS;
        }
        else
        { // dst가 src와 다른 경우 삭제
            if (bi_delete(dst) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
        }
    }

    int mod_word = mod_len / WORD_BITS; // mod_len에 대한 word_len
    int mod_bit = mod_len % WORD_BITS;  // bit 단위로 시프트

    int new_word_len = mod_word + (mod_bit > 0); // 새로 할당할 bigint 길이
    if (bi_new(dst, new_word_len) != BI_ALLOC_SUCCESS)
        return BI_MOD_FAIL; // 새로운 길이 맞게 할당
    (*dst)->sign = (*src)->sign;

    // word 이동
    for (int i = 0; i < new_word_len; i++)
    {
        (*dst)->a[i] = (*src)->a[i];
    }

    // bit 이동
    if (mod_bit != 0)
    {
        (*dst)->a[new_word_len - 1] = (*dst)->a[new_word_len - 1] & ((1 << mod_bit) - 1);
    }

    return BI_MOD_SUCCESS;
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
msg bi_cat(OUT bigint **dst, IN bigint **a, IN bigint **b)
{
    int new_word_len = 0;

    // 두 값 중 하나가 NULL인 경우
    if (*a == NULL || *b == NULL)
        return BI_CAT_FAIL;
    // 연접하려는 두 값의 부호가 다를 경우
    if ((*a)->sign != (*b)->sign)
        return BI_SIGN_NOT_MATCH;

    // dst가 비어있지 않을 경우
    if (*dst != NULL)
    {
        // 만약 src와 dst가 동일한 경우 수행한 값을 src에 저장하여 반환
        if (*dst == *a || *dst == *b)
        {
            bigint *temp = NULL;
            if (bi_cat(&temp, a, b) != BI_SHIFT_SUCCESS)
                return BI_CAT_FAIL;
            if (bi_assign(dst, &temp) != BI_SET_ASSIGN_SUCCESS)
                return BI_CAT_FAIL;
            if (bi_delete(&temp) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
            return BI_CAT_SUCCESS;
        }
        else
        { // dst가 src와 다른 경우 삭제
            if (bi_delete(dst) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
        }
    }

    new_word_len = (*a)->word_len + (*b)->word_len;
    if (bi_new(dst, new_word_len) != BI_ALLOC_SUCCESS)
        return BI_CAT_FAIL; // 새로운 길이 맞게 할당
    (*dst)->sign = (*a)->sign;
    // b 값 먼저 넣기
    for (int i = 0; i < (*b)->word_len; i++)
        (*dst)->a[i] = (*b)->a[i];
    // 이후 a 값 넣기
    for (int i = 0; i < (*a)->word_len; i++)
        (*dst)->a[i + (*b)->word_len] = (*a)->a[i];

    return BI_CAT_SUCCESS;
}

bool bi_is_zero(const bigint *num)
{
    if (num == NULL || num->word_len == 0)
    {
        return true; // NULL이거나 길이가 0인 경우 0으로 간주
    }
    for (int i = 0; i < num->word_len; i++)
    {
        if (num->a[i] != 0)
        {
            return false; // 하나라도 0이 아니면 false 반환
        }
    }
    return true; // 모든 요소가 0이면 true 반환
}