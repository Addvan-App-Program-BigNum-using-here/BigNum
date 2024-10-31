#include "random.h"

msg bi_get_random(bigint** dst, int word_len){
    msg result_msg = 0;
    printf("%d\n", word_len);
	result_msg = bi_new(dst, word_len);
    if (result_msg != BI_ALLOC_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }

    (*dst)->sign = rand() & 1;

    result_msg = array_rand((*dst)->a, word_len);
    if(result_msg != GEN_RANDOM_SUCCESS)
        return GEN_RANDOM_FAIL;

    result_msg = bi_refine(*dst);
    if(result_msg != BI_SET_REFINE_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }

    return BI_GET_RANDOM_SUCCESS;
}


msg array_rand(word* dst, int word_len){
    byte* p = (byte*) dst;
    int cnt = word_len * (sizeof(word) / sizeof(byte));
    while (cnt > 0)
    {
        *p = rand() & 0xff; 
        p++;
        cnt--;
    }

    return GEN_RANDOM_SUCCESS;
}

