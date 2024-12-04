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
int bigint_to_hex(OUT char* str, IN bigint** src) {
    if (*src == NULL) return -1; // NULL 체크

    int idx = 0;
    int buffer_size = 0;
    const char* format = NULL; // 포맷 문자열 초기화

    // WORD_BITS에 따른 형식과 버퍼 크기 설정
    switch (WORD_BITS) {
        case 64:
            format = "%016" PRIx64;
            buffer_size = 17;
            break;
        case 32:
            format = "%08" PRIx32;
            buffer_size = 9;
            break;
        case 16:
            format = "%04" PRIx16;
            buffer_size = 5;
            break;
        case 8:
            format = "%02" PRIx8;
            buffer_size = 3;
            break;
        default:
            return -2; // Unsupported WORD_BITS
    }

    // 부호 처리
    if ((*src)->sign == 1) {
        str[idx++] = '-';
    }

    // 16진수 접두사 추가
    str[idx++] = '0';
    str[idx++] = 'x';

    // 각 워드 값을 출력
    for (int i = (*src)->word_len - 1; i >= 0; i--) {
        idx += snprintf(&str[idx], buffer_size, format, (*src)->a[i]);
    }

    // 문자열 종료 처리
    str[idx] = '\0';

    return idx; // 최종 문자열 길이 반환
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
    // 입력된 byte 크기가 4보다 클 경우
    if(byte_len > 4)    return 0;
    word result = 0;
    for(int i = 0; i < byte_len; i++){
        if(result > MAX_VALUE) return 0;
        result = result * 256 + input[i];
    }
    return result;
}

/*************************************************
* Name:        int_to_string
*
* Description: Transfer integer to string
*
* Arguments:   - char* str: char array
*              - int num: integer
**************************************************/
char* int_to_string(IN int num){
    char* str = (char*)calloc(12, sizeof(char));
    snprintf(str, 12, "%d", num);
    return str;
}