#include "data_type.h"

/*************************************************
* Name:        char_to_int
*
* Description: Transfer char to integer
*
* Arguments:   - char str : string
* Return:      - int : integer
**************************************************/
int char_to_int(IN char str){
    if (str >= '0' && str <= '9')
            return str - '0';
    else if (str >= 'A' && str <= 'F')
            return str - 'A' + 10;
    else if (str >= 'a' && str <= 'f')
            return str - 'a' + 10;
    else
            return -1;
}

/*************************************************
* Name:        bigint_to_hex
*
* Description: Transfer bigint to hex string
*
* Arguments:   - bigint** src: bigint structure
*              - char* str: store string
* Return:      - int : result of integer, FAIL == -1
**************************************************/
int bigint_to_hex(IN bigint** src, OUT char* str) {
    // Assuming the bigint is stored in little-endian format
    int idx = 0;

    // src가 사용되고 있지 않은 경우
    if(*src == NULL)    return -1;

    idx += ((*src)->sign == 1) ? snprintf(&str[idx], 2, "-") : 0; // 부호 값 저장
    idx += snprintf(&str[idx], 3, "0x");
    for (int i = (*src)->word_len - 1; i >= 0; i--) {
        idx += snprintf(&str[idx], 9, "%08X", (*src)->a[i]);
    }
    return idx;
}

/*************************************************
* Name:        byte_to_uint
*
* Description: Transfer byte to unsinged integer
*
* Arguments:   - byte* input: bytes array
*              - int byte_len: length of bytes array
* Return:      - word : integer. FAIL == 0
**************************************************/
word byte_to_uint(IN byte* input, const IN int byte_len){
    word result = 0;
    // 입력된 byte 크기가 4보다 클 경우
    if(byte_len > 4)    return 0;

    for(int i = 0; i < byte_len; i++){
        if(result > UINT32_MAX) return 0;
        result = result * 256 + input[i];
    }
    return result;
}