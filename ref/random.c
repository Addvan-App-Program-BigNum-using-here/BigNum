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
        log_msg(result_msg);
        return result_msg;
    }

    return BI_GET_RANDOM_SUCCESS;
}


/*************************************************
* Name:        array_random
*
* Description: Fill array with random values
*
* Arguments:   - word* dst: pointer to bigint struct
*              - int word_len: length of bigint struct
**************************************************/
msg array_rand(word* dst, int word_len){
    int byte_len = 0;
    byte_len = word_len * (sizeof(word) / sizeof(byte)); 
    if (RAND_bytes((byte*)dst, byte_len) != 1) {
        return GEN_RANDOM_FAIL;
    }

    return GEN_RANDOM_SUCCESS;
}


/*************************************************
* Name:        get_random_string
*
* Description: Create Random String with given length
*
* Arguments:   - char* str: return String
*              - int str_len : length of return String
*              - int base : base of return String
**************************************************/
msg get_random_string(OUT char* str, IN int str_len, IN int base){
    int str_idx = str_len;
// 여기도 str 메모리 측면에서 문제가 있을 수 있음
    // 초기화 -> DRBG를 적용해야 가능
//    srand(time(NULL));

    // 각 진수별 사용할 문자 배열
    const char binary_chars[] = "01";
    const char decimal_chars[] = "0123456789";
    const char hex_chars[] = "0123456789abcdef";
    const char* chars;
    int chars_len;

    switch(base) {
        case 2:
            chars = binary_chars;
            chars_len = sizeof(binary_chars) - 1;
            break;
        case 10:
            chars = decimal_chars;
            chars_len = sizeof(decimal_chars) - 1;
            break;
        case 16:
            chars = hex_chars;
            chars_len = sizeof(hex_chars) - 1;
            break;
        default:
            str[0] = '\0';
            return RAND_STRING_INVALID;
    }

    while(str_idx > 0) {
        int random_index = rand() % chars_len;
        if(strncmp(&chars[random_index], "0", 1) == 0 && str_idx == str_len)    continue;
        str[str_idx - 1] = chars[random_index];
        str_idx--;
    }

    str[str_len] = '\0';

    return RAND_STRING_SUCCESS;
}
