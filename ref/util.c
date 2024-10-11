#include "util.h"

/*************************************************
* Name:        bi_new
*
* Description: New allocate memory for bigint struct
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - int word_len: length of bigint struct
**************************************************/
msg bi_new(bigint** dst, int word_len){
    if(*dst != NULL){
        bi_delete(dst);
    }
    *dst = (bigint*)calloc(1, sizeof(bigint));
    if(*dst == NULL){
        return BI_ALLOC_FAIL;
    }
    (*dst)->sign = 0;
    (*dst)->a = (word*)calloc(word_len, sizeof(word));
    if((*dst)->a == NULL){
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
**************************************************/
msg bi_delete(bigint** dst){
    if(*dst == NULL){
        return BI_FREE_SUCCESS;
    }

    if((*dst)->a == NULL){
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
**************************************************/
msg bi_set_from_array(bigint** dst, int sign, int word_len, word* data, int endian){
    msg result_msg = 0;
    int endian_idx = endian ? 0 : word_len - 1;
    int idx = endian ? 1 : -1;

    result_msg = bi_new(dst, word_len);
    if(result_msg == BI_ALLOC_FAIL){
        print_log(result_msg);
        return result_msg;
    }

    if(dst == NULL){
        return MEM_NOT_ALLOC;
    }

    (*dst)->sign = sign;
    for(int i = 0; i < word_len; i++){
        if(data[endian_idx] > 0xFFFFFFFF){
            printf("DATA_OVERFLOW\n");
            result_msg = bi_delete(dst);
            if(result_msg != BI_FREE_SUCCESS){
                print_log(result_msg);
                return result_msg;
            }
            return BI_SET_ARRAY_FAIL;
        }
        (*dst)->a[i] = data[endian_idx];
        endian_idx += idx;
    }

// 확인 필요
//    result_msg = bi_refine(*dst);
//    if(result_msg != BI_SET_REFINE_SUCCESS){
//        print_log(result_msg);
//        return result_msg;
//    }

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
**************************************************/
msg bi_set_from_string(bigint** dst, char* int_str, int base){
    msg result_msg = 0;
    int sign = 0, word_len = 0, word_idx = 0;
    word remain = 0;
    uint64_t q = 0;

    // sign bit check
    if(int_str[0] == '-'){
        sign = 1;
        int_str++;
    }
    // 16진수 접두사 처리
    if (base == 16 && int_str[0] == '0' && (int_str[1] == 'x' || int_str[1] == 'X')) {
        int_str += 2;
    }

    // extract word length
    switch (base) {
        case 2:
            word_len = (strlen(int_str) + 31) / 32; // ceil 함수 사용하지 않고 구현
            break;
        case 10:
            word_len = (strlen(int_str) + 9) / 10; // ceil 함수 사용하지 않고 구현
            break;
        case 16:
            word_len = (strlen(int_str) + 7) / 8; // ceil 함수 사용하지 않고 구현
            break;
        default:
            return BI_SET_ARRAY_FAIL;
    }

    result_msg = bi_new(dst, word_len);
    if(result_msg == BI_ALLOC_FAIL){
        print_log(result_msg);
        return result_msg;
    }
    (*dst)->sign = sign;


    do{
        result_msg = String_Divide(int_str, &remain, base);
        if(result_msg != DIVIDE_STRING_SUCCESS){
            print_log(result_msg);
            return result_msg;
        }
        (*dst)->a[word_idx++] = remain;
        q = string_to_int(int_str, base);
    }while(q > 0xFFFFFFFF);
    (*dst)->a[word_idx++] = q;


    for(int i = 0 ; i < word_len; i++){
        printf("0x%08x\n", (*dst)->a[i]);
    }

//  확인 필요
//    result_msg = bi_refine(*dst);
//    if(result_msg != BI_SET_REFINE_SUCCESS){
//        print_log(result_msg);
//        return result_msg;
//    }

        return BI_SET_STRING_SUCCESS;
}


/*************************************************
* Name:        string_to_int
*
* Description: Convert string to integer
*
* Arguments:   - char* int_str: string of bigint
*              - word* result: result of integer
**************************************************/
uint64_t string_to_int(char* str, int base){
    int i = 0;
    uint64_t num = 0;
    while (str[i] != '\0'){
        if (str[i] >= '0' && str[i] <= '9'){
            num = num * base + (str[i] - '0');
            i++;
        }
    }
    return num;
}

/*************************************************
* Name:        String_Divide
*
* Description: Divide string to word
*
* Arguments:   - char* int_str: string of bigint
*              - int base: base of string (2, 10, 16)
**************************************************/
msg String_Divide(char* int_str, word* r, int base){
    int idx = 0;
    uint64_t temp = 0;
    char* q = NULL;

    q = (char*)calloc(strlen(int_str) + 1, sizeof(char));
    if(q == NULL){
        return BI_ALLOC_FAIL;
    }

    for (int i = 0; int_str[i] != '\0'; i++) {
        int digit;
        if (int_str[i] >= '0' && int_str[i] <= '9')
                digit = int_str[i] - '0';
        else if (int_str[i] >= 'A' && int_str[i] <= 'F')
                digit = int_str[i] - 'A' + 10;
        else if (int_str[i] >= 'a' && int_str[i] <= 'f')
                digit = int_str[i] - 'a' + 10;
        else
                continue; // 잘못된 문자는 무시 혹은 오류 인데 일단 무시

        temp = temp * base + digit;
        if(temp > 0xFFFFFFFF){
            int_to_char(temp >> WORD_BITS, q, idx++);
            temp = temp % 0x100000000; // 나머지 가져오기
        }
    }
    memset(int_str, '0', strlen(int_str));
    strncpy(int_str, q, strlen(q));
    int_str[strlen(q)] = '\0';
    *r = (word)temp;

    return DIVIDE_STRING_SUCCESS;
}

int int_to_char(int num, char *str, int idx){
    while (num != 0){
        str[idx] = num % 10 + '0';
        num /= 10;
    }
    return 0;
}

/*************************************************
* Name:        bi_refine
*
* Description: Refine bigint struct
*
* Arguments:   - bigint* src: pointer to bigint struct
**************************************************/
msg bi_refine(bigint *src)
{
    if (src == NULL)
    {
        return BI_SET_REFIEN_FAIL;
    }

    int new_wordlen = src->word_len;
    while(new_wordlen > 1){
        if(src->a[new_wordlen - 1] != 0){
            break;
        }
        new_wordlen--;
    }
    if (src->word_len != new_wordlen)
    {
        src->word_len = new_wordlen;
        src->a = (word*)realloc(src->a, new_wordlen * sizeof(word));
    }

    if((src->word_len == 1) && (src->a[0] == 0)){
        src->sign = 0;
    }
    return BI_SET_REFINE_SUCCESS;
}


/*************************************************
* Name:        bi_assign
*
* Description: Processsing_assign bigint struct
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - bigint* src: source bigint struct
**************************************************/
msg bi_assign(bigint** dst, bigint *src)
{
    msg result_msg = 0;
    if (*dst == src)
        return BI_SET_ASSIGN_SUCCESS; // 자기 자신에게 할당하는 경우

    result_msg = bi_delete(dst);
    if (result_msg != BI_FREE_SUCCESS)
    {
        print_log(result_msg);
        return result_msg;
    }

    result_msg = bi_new(dst, src->word_len);
    if (result_msg != BI_FREE_SUCCESS)
        return result_msg;

    (*dst)->sign = src->sign;
    memcpy((*dst)->a, src->a, src->word_len * sizeof(word));

    return BI_SET_ASSIGN_SUCCESS;
}

/*************************************************
* Name:        bi_print
*
* Description: Print bigint struct
*
* Arguments:   - bigint* dst: pointer to bigint struct
*              - int base: base of bigint struct (2, 10, 16)
**************************************************/
msg bi_print(bigint* dst, int base)
{
    if (dst == NULL || dst->a == NULL)
        return PRINT_NULL;

    if (dst->sign == 0)
    {
        printf("0\n");
        return 0;
    }

    if (dst->sign < 0)
        printf("-");
    if (base == 16)
        printf("0x");

    // 간단한 16진수 출력 (10 진수는 이후 추가)
    for (int i = dst->word_len - 1; i >= 0; i--)
    {
        printf("%08x", dst->a[i]);
    }
    printf("\n");

    return 0;
}