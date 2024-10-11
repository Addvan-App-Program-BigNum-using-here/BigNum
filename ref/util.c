#include "util.h"

/*************************************************
 * Name:        bi_new
 *
 * Description: New allocate memory for bigint struct
 *
 * Arguments:   - bigint** dst: pointer to bigint struct
 *              - int word_len: length of bigint struct
 **************************************************/
msg bi_new(bigint **dst, int word_len)
{
    if (*dst != NULL)
    {
        bi_delete(dst);
    }
    *dst = (bigint *)calloc(1, sizeof(bigint));
    if (*dst == NULL)
    {
        return BI_ALLOC_FAIL;
    }
    (*dst)->sign = 0;
    (*dst)->a = (word *)calloc(word_len, sizeof(word));
    if ((*dst)->a == NULL)
    {
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
msg bi_delete(bigint **dst)
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
 *              - bool endian: little endian == 0, big endian == 1
 **************************************************/
msg bi_set_from_array(bigint **dst, int sign, int word_len, word *data, int endian)
{
    msg result_msg = 0;
    int endian_idx = endian ? 0 : word_len - 1;
    int idx = endian ? 1 : -1;

    result_msg = bi_new(dst, word_len);
    if (result_msg == BI_ALLOC_FAIL)
    {
        print_log(result_msg);
        return result_msg;
    }

    if (dst == NULL)
    {
        return MEM_NOT_ALLOC;
    }

    (*dst)->sign = sign;
    for (int i = 0; i < word_len; i++)
    {
        if (data[endian_idx] > 0xFFFFFFFF)
        {
            printf("DATA_OVERFLOW\n");
            bi_delete(dst);
            return BI_SET_ARRAY_FAIL;
        }
        (*dst)->a[i] = data[endian_idx];
        endian_idx += idx;
    }
    return BI_SET_ARRAY_SUCCESS;
}

//
///*************************************************
//* Name:        bi_set_from_string
//*
//* Description: Set bigint struct from string
//*
//* Arguments:   - bigint** dst: pointer to bigint struct
//*              - char* int_str: string of bigint
//*              - int base: base of string (2, 10, 16)
//**************************************************/
// msg bi_set_from_string(bigint** dst, char* int_str, int base){
//    int result_msg = 0;
//    int sign = 0;
//    int word_len = 0;
//
//    // sign bit check
//    if(int_str[0] == '-'){
//        sign = 1;
//        int_str++;
//    }
//
//    // extract word length
//    switch (base) {
//        case 2:
//            word_len = (strlen(int_str) + 31) / 32; // ceil 함수 사용하지 않고 구현
//            break;
//        case 10:
//            word_len = (strlen(int_str) + 9) / 10; // ceil 함수 사용하지 않고 구현
//            break;
//        case 16:
//            word_len = (strlen(int_str) + 7) / 8; // ceil 함수 사용하지 않고 구현
//            break;
//        default:
//            return BI_SET_ARRAY_FAIL;
//    }
//
//    result_msg = bi_new(dst, word_len);
//    if(result_msg == BI_ALLOC_FAIL){
//        print_log(result_msg);
//        return result_msg;
//    }
//    (*dst)->sign = sign;
//
//}
///*************************************************
//* Name:        bi_assign
//*
//* Description: Processsing_assign bigint struct
//*
//* Arguments:   - bigint** dst: pointer to bigint struct
//*              - bigint* src: source bigint struct
//**************************************************/
msg bi_assign(bigint **dst, bigint *src)
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

msg bi_print(bigint *dst, int base)
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