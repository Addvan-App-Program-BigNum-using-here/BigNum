/*
    2024.10.18.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// typedef unsigned int word;
typedef uint32_t word;
typedef uint32_t msg;

// 4바이트
#define WORD_BITS 32

typedef struct
{
    int sign; // 1 : 양수, -1 : 음수, 0 : 0
    int word_len;
    word *a;
} bigint;

msg bi_new(bigint **dst, int word_len);
// 델레트가 비밀값이라면 반납할때 0으로초기화같은 것을 해줘야한다.
msg bi_delete(bigint **dst);

/* bigint <- array */
msg bi_set_from_array(bigint **dst, int sign, int len, word *data);

msg bi_refine(bigint *dst);

/* bigint <- string /
/ str = 0x123qqpp 16진수로 표기할 수 없는 것들 예외처리 */
msg bi_set_from_string(bigint **dst, char *int_str, int base);

/* 출력 방법 두가지 /
/ 1. 0x1234, 2.1234 */
// 10진수 출력하려면 나머지가 필요하다.
msg bi_print(bigint *dst, int base);
/* tmp <- x */
msg bi_assign(bigint **dst, bigint *src);

/* A[4] = {1,2,3,0}  word len은 무조건 0이 아니어야 함 왜냐하면 0이 마지막값이면 3으로 측정하기 때문에, /
/ A[4] = {1, 2, 3, 0} -> {1, 2, 3, 1} */
msg bi_get_random(bigint *dst, int wordl_len);

int main()
{
    bigint *a = NULL;
    bigint *b = NULL;

    printf("Setting a from string...\n");
    bi_set_from_string(&a, "0x12345678", 16);
    printf("Printing a:");
    bi_print(a, 16);

    printf("Setting b from string...\n");
    bi_set_from_string(&b, "12345678", 10);
    printf("Printing b:");
    bi_print(b, 16);

    printf("Assigning b to a...\n");
    bi_assign(&a, b);
    printf("Printing a after assignment:\n");
    bi_print(a, 16);

    printf("Deleting a and b...\n");
    bi_delete(&a);
    bi_delete(&b);
}

msg bi_new(bigint **dst, int word_len)
{

    *dst = (bigint *)calloc(1, sizeof(bigint));
    if (*dst == NULL)
    {
        return 1; // 메모리 할당 실패
    }

    (*dst)->a = (word *)calloc(word_len, sizeof(word));
    if ((*dst)->a == NULL)
    {
        free(*dst);
        *dst = NULL;
        return 1; // 메모리 할당 실패
    }

    (*dst)->word_len = word_len;
    (*dst)->sign = 0; // calloc이 0으로 초기화해주기 때문에 0으로 초기화할 필요가 없지만. 명시적으로 써줌.
    return 0;
}

msg bi_delete(bigint **dst)
{
    if (*dst == NULL)
        return 0; // 이미 할당 해제되어있음

    if ((*dst)->a != NULL)
    {
        memset((*dst)->a, 0, (*dst)->word_len * sizeof(word)); // 보안을 위해 0으로 초기화
        free((*dst)->a);
    }
    free(*dst);
    *dst = NULL;
    return 0;
}

msg bi_refine(bigint *dst)
{
    int i;
    for (i = dst->word_len - 1; i >= 0; i--)
    {
        if (dst->a[i] != 0)
            break;
    }
    dst->word_len = i + 1;
    // 모든 word가 0이면 부호도 0
    if (dst->word_len == 0)
        dst->sign = 0;

    return 0;
}

msg bi_set_from_array(bigint **dst, int sign, int len, word *data)
{
    msg result = bi_new(dst, len);
    if (result != 0)
        return result;

    (*dst)->sign = (sign > 0) ? 1 : ((sign < 0) ? -1 : 0);
    memcpy((*dst)->a, data, len * sizeof(word));
    return bi_refine(*dst);
}

msg bi_set_from_string(bigint **dst, char *int_str, int base)
{
    int i = 0;
    int len = 0;
    int sign = 1;

    // 부호 처리
    if (int_str[0] == '-')
    {
        sign = -1;
        i++;
    }
    else if (int_str[0] == '+')
    {
        i++;
    }

    // 16진수 접두사 처리
    if (base == 16 && int_str[i] == '0' && (int_str[i + 1] == 'x' || int_str[i + 1] == 'X'))
    {
        i += 2;
    }

    // 문자열 길이 계산

    len = strlen(int_str + i);
    int word_len = (len + (WORD_BITS / 4) - 1) / (WORD_BITS / 4); // 16진수 기준으로 계산

    msg result = bi_new(dst, word_len);
    if (result != 0)
        return result;

    (*dst)->sign = sign;

    // 문자열을 bigint로 변환
    for (; int_str[i] != '\0'; i++)
    {
        int digit;
        if (int_str[i] >= '0' && int_str[i] <= '9')
            digit = int_str[i] - '0';
        else if (int_str[i] >= 'A' && int_str[i] <= 'F')
            digit = int_str[i] - 'A' + 10;
        else if (int_str[i] >= 'a' && int_str[i] <= 'f')
            digit = int_str[i] - 'a' + 10;
        else
            continue; // 잘못된 문자는 무시 혹은 오류 인데 일단 무시

        // 기존 값에 base를 곱하고 새 digit를 더함
        for (int j = 0; j < (*dst)->word_len; j++)
        {
            uint64_t temp = (uint64_t)(*dst)->a[j] * base + (j == 0 ? digit : 0);
            (*dst)->a[j] = (word)temp;
            if (j < (*dst)->word_len - 1)
                (*dst)->a[j + 1] = (word)(temp >> WORD_BITS);
        }
    }
    return bi_refine(*dst);
}

msg bi_print(bigint *dst, int base)
{
    if (dst == NULL || dst->a == NULL)
        return 1;

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

msg bi_assign(bigint **dst, bigint *src)
{
    if (*dst == src)
        return 0; // 자기 자신에게 할당하는 경우

    bi_delete(dst); // 기존 dst 메모리 해제

    msg result = bi_new(dst, src->word_len);
    if (result != 0)
        return result;

    (*dst)->sign = src->sign;
    memcpy((*dst)->a, src->a, src->word_len * sizeof(word));

    return 0;
}