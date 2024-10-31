#include "util.h"

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

    result_msg = bi_refine(*dst);
    if(result_msg != BI_SET_REFINE_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }

    return BI_SET_ARRAY_SUCCESS;
}

msg bi_set_from_string(bigint** dst, char* int_str, int base){
    msg result_msg = 0;
    int sign = 0, word_idx = 0;
    word word_len = 0;
    dword q = 0;

    // sign bit check
    if(int_str[0] == '-'){
        sign = 1;
        int_str++;
    }

    // extract word length
    switch (base) {
        case 2:
            if(int_str[0] != '0' || int_str[1] != 'b'){
                printf("bit string Input Must be start '0b'!!\nstart: %c%c\n", int_str[0], int_str[1]);
                return BI_SET_STRING_FAIL;
            }
            int_str += 2;
            word_len = (strlen(int_str) + 31) / 32;
            break;
        case 10:
            word_len = (strlen(int_str) + 9) / 10;
            break;
        case 16:
            if(int_str[0] != '0' || int_str[1] != 'x'){
                printf("hex string Input Must be start '0x'\nstart: %c%c\n", int_str[0], int_str[1]);
                return BI_SET_STRING_FAIL;
            }
            int_str += 2;
            word_len = (strlen(int_str) + 7) / 8;
            break;
        default:
            printf("%d base is not supported\n", base);
            return BI_SET_STRING_FAIL;
    }

    // memory allocate
    result_msg = bi_new(dst, word_len);
    if(result_msg != BI_ALLOC_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }
    // sign bit set
    (*dst)->sign = sign;
    do{
        result_msg = String_Divide(int_str, ((*dst)->a) + (word_idx++), base);
        if(result_msg != DIVIDE_STRING_SUCCESS){
            print_log(result_msg);
            return result_msg;
        }
        q = string_to_int(int_str, base);
    }while(q > 0xFFFFFFFF);
    if(q != 0)
        (*dst)->a[word_idx++] = q;

    result_msg = bi_refine(*dst);
    if(result_msg != BI_SET_REFINE_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }

        return BI_SET_STRING_SUCCESS;
}

msg String_Divide(char* int_str, word* a, int base){
    int q_idx = 0, digit = 0, a_idx = 0;
    dword temp = 0;
    char* q = NULL;

    q = (char*)calloc(strlen(int_str) + 1, sizeof(char));
    if(q == NULL){
        return BI_ALLOC_FAIL;
    }

    for (int i = 0; int_str[i] != '\0'; i++) {
        digit = char_to_int(int_str[i]);
        if(digit == -1){
            printf("Invalid character\n");
            return DIVIDE_STRING_FAIL;
        }

        temp = temp * base + digit;
        if(temp > 0xFFFFFFFF){
            switch (base) {
                case 2:
                    *(a+(a_idx++)) = (word)(temp >> 1);
                    temp = temp & 0x1;
                    break;
                case 10:
                    int_to_char((word)(temp >> WORD_BITS), q, q_idx++);
                    temp = temp % 0x100000000; // 나머지 가져오기
                    break;
                case 16:
                    *(a+(a_idx++)) = (word)(temp >> 4);
                    temp = temp & 0xF;
                    break;
                default:
                    printf("%d base is not supported\n", base);
                    return DIVIDE_STRING_FAIL;
            }
        }
    }
    memset(int_str, '0', strlen(int_str));
    strncpy(int_str, q, strlen(q));
    int_str[strlen(q)] = '\0';
    *(a+(a_idx)) = (word)temp;

    return DIVIDE_STRING_SUCCESS;
}

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

msg bi_print(bigint** dst, int base)
{
    if (dst == NULL || (*dst)->a == NULL)
        return PRINT_NULL;

    if ((*dst)->sign == 0)
    {
        printf("0\n");
        return 0;
    }

    if ((*dst)->sign < 0)
        printf("-");
    if (base == 16)
        printf("0x");

    // 간단한 16진수 출력 (10 진수는 이후 추가)
    for (int i = (*dst)->word_len - 1; i >= 0; i--)
    {
        printf("%08x", (*dst)->a[i]);
    }
    printf("\n");

    return 0;
}