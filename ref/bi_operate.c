#include "data_type.h"
#include "error_msg.h"
#include "util.h"
#include "random.h"

bigint bi_add(bigint* A, bigint* B)
{
    if ((A == NULL) || (B == NULL))
    {
       // return BI_ADD_FAIL;
    }

    int wordlen_A, wordlen_B, wordlen_C = 0;
    int sign_A, sign_B = 0;
    bigint* C;

    wordlen_A = A->word_len;
    wordlen_B = B->word_len;

    if(wordlen_A >= wordlen_B)
    {
        C->word_len = wordlen_A;
    }
    else
    {
        return bi_add(B,A); // 추후 코드를 wordlen_A >= wordlen_B 가정하에 작성할 것임
    }
    wordlen_C = C->word_len;

    sign_A = A->sign;
    sign_B = B->sign;

    if((sign_A == 1) && (sign_B == 0))
    {
        A->sign = 0;
       // return bi_sub(B,A);
    }

    if((sign_A == 0) && (sign_B == 1))
    {
        B->sign = 0;
        //return bi_sub(A,B);
    }

    if((sign_A == 1) && (sign_B == 1))
    {
        B->sign = 0;
        //return bi_sub(A,B);
    }
    C->sign = 0;

    msg result_msg = 0;

    result_msg = bi_new(C, (C->word_len));
    if(result_msg != BI_ALLOC_SUCCESS)
    {
        print_log(result_msg);
        //return result_msg;
    }

    int i, j = 0;
    int c1, c2 = 0;
    int add_result = 0;

    for(i = wordlen_A-1, j = wordlen_B-1; j >= 0; i--, j--)
    {   
        // c1, c2 변수 2개 선언 후 carry가 발생함에 따라 업데이트 되는 것은 c2
        c1 = c2;

        C->a[i] = A->a[i] + B->a[j];
        if(C->a[i] < A)
        {
            c2 = 1;
        }
        else
        {
            c2 = 0;
        }

        if((C->a[i] + c1) < c1)
        {
            c2 = 1;
        }
        
    }
    i = wordlen_B;

    for(i = wordlen_B; i >= 0; i--)
    {
        C->a[i] = A->a[i] + c2;
        if(A->a[i] + c2 < c2)
        {
            c2 = 1;
        }
        else
        {
            c2 = 0;
        }
    }

    if(c2 == 1)
    {
        wordlen_C += 1;
        C->a = (word*)realloc(C->a, wordlen_C * sizeof(word));
        C->a[0] = 1;
    }

    return *C;

}

msg bi_mul(bigint** dst, bigint**A, bigint** B){
    msg result_msg = 0;
    
    if ((A == NULL) || (B == NULL))  //return bi_mul_fail;

    (*dst)->word_len = (*A)->word_len + (*B)->word_len; 

    result_msg = bi_new(dst, (*dst)->word_len);
    if(result_msg != BI_ALLOC_SUCCESS)
    {
        print_log(result_msg);
        return result_msg;
    }
    
}


