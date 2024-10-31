#include "random.h"

/*************************************************
* Name:        bi_get_random
*
* Description: New allocate bigint struct and fill it with random values
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - int word_len: length of bigint struct
**************************************************/
msg bi_get_random(OUT bigint** dst, const IN int word_len) {
	int result_msg;

	result_msg = bi_new(dst, word_len);
	if (result_msg != BI_ALLOC_SUCCESS) {
	    log_msg(result_msg);
        return result_msg;
    }

//    (*dst)->sign = rand() & 1;
    (*dst)->sign = 0;
    result_msg = array_rand((*dst)->a, word_len);
    if(result_msg != GEN_RANDOM_SUCCESS)
        return GEN_RANDOM_FAIL;

    result_msg = bi_refine(*dst);
    if(result_msg != BI_SET_REFINE_SUCCESS){
        log_msg(result_msg);
        return result_msg;
    }

    return BI_GET_RANDOM_SUCCESS;
}


/*************************************************
* Name:        array_rand
*
* Description: Fill array with random values
*
* Arguments:   - word* dst: pointer to bigint struct
*              - int word_len: length of bigint struct
**************************************************/
msg array_rand(OUT word* dst, const IN int word_len){
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