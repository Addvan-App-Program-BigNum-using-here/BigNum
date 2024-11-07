#include "random.h"

msg bi_get_random(bigint** dst, int word_len){
    msg result_msg = 0;
    printf("%d\n", word_len);
	result_msg = bi_new(dst, word_len);
    if (result_msg != BI_ALLOC_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }

    // if(RAND_status() == 0){
    //     RAND_poll();
    //     RAND_status();
    // }
    if(RAND_bytes((byte*)&(*dst)->sign, sizeof((*dst)->sign)) != 1){
        return BI_GET_RANDOM_FAIL;
    }
    (*dst)->sign = (*dst)->sign & 1;

    result_msg = array_rand((*dst)->a, word_len);
    if(result_msg != GEN_RANDOM_SUCCESS)
        return result_msg;

    result_msg = bi_refine(*dst);
    if(result_msg != BI_SET_REFINE_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }

    return BI_GET_RANDOM_SUCCESS;
}


msg array_rand(word* dst, int word_len){
    int byte_len = 0;
    byte_len = word_len * (sizeof(word) / sizeof(byte)); 
    if (RAND_bytes((byte*)dst, byte_len) != 1) {
        return GEN_RANDOM_FAIL;
    }

    return GEN_RANDOM_SUCCESS;
}
