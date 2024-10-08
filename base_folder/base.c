/*
    2024.10.18.
*/

#include <stdio.h>
#include <stdint.h>

typedef unsigned int word;
typedef uint32_t word;

typedef uint32_t msg;

typedef struct
{
    int sign;
    int word_len;
    word *a;
} bigint;

/* bigint <- array */
msg bi_set_from_array(bigint *dst, int sign, int len, word *data);

/* bigint <- string /
/ str = 0x123qqpp 16진수로 표기할 수 없는 것들 예외처리 */
msg bi_set_from_string(bigint *dst, char *int_str, int base);

/* A[4] = {1,2,3,0}  word len은 무조건 0이 아니어야 함 왜냐하면 0이 마지막값이면 3으로 측정하기 때문에, /
/ A[4] = {1, 2, 3, 0} -> {1, 2, 3, 1} */
msg bi_get_random(bigint *dst, int wordl_len);

/* 출력 방법 두가지 /
/ 1. 0x1234, 2.1234 */
// 10진수 출력하려면 나머지가 필요하다.
msg bi_print(bigint *dst, int base);

msg bi_new(bigint *dst, int word_len);
// 델레트가 비밀값이라면 반납할때 0으로초기화같은 것을 해줘야한다.
msg bi_delete(bigint *dst);

msg bi_refine(bigint *dst);

/* tmp <- x */
msg bi_assign(bigint *dst, bigint *src);

int main()
{

    bigint a, b, c;

    /*
        a = -0x1231231231233
        b = -0x1f31231231233
        a = b
    */


    return 0;
}