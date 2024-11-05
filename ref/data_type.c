#include "data_type.h"

/*************************************************
* Name:        string_to_int
*
* Description: Convert string to integer
*
* Arguments:   - char* int_str: string of bigint
*              - word* result: result of integer
**************************************************/
dword string_to_int(IN char* str, const IN int base){
    int i = 0;
    dword num = 0;
    while (str[i] != '\0'){
        if (str[i] >= '0' && str[i] <= '9'){
            num = num * base + (str[i] - '0');
            i++;
        }
    }
    return num;
}

/*************************************************
* Name:        char_to_int
*
* Description: Transfer char to integer
*
* Arguments:   - char str : string
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
* Name:        int_to_char
*
* Description: Transfer integer to string
*
* Arguments:   - int num: integer
*              - char* str: string
*              - int idx: index of string
**************************************************/
void int_to_char(IN word num, OUT char *str, IN int idx){
    while (num != 0){
        str[idx] = num % 10 + '0';
        num /= 10;
    }
}

/*************************************************
* Name:        bigint_to_hex
*
* Description: Transfer bigint to hex string
*
* Arguments:   - bigint** src: bigint structure
*              - char* str: store string
**************************************************/
int bigint_to_hex(IN bigint** src, OUT char* str) {
    // Assuming the bigint is stored in little-endian format
    int idx = 0;

    if(*src == NULL)    return 0;
    idx += ((*src)->sign == 1) ? sprintf(&str[idx], "-") : 0; // 부호 값 저장
    idx += sprintf(&str[idx], "0x");
    for (int i = (*src)->word_len - 1; i >= 0; i--) {
        idx += sprintf(&str[idx], "%08X", (*src)->a[i]);
    }
    return idx;
}

/*************************************************
* Name:        operate_string_cat
*
* Description: make string from bigint string and operator
*
* Arguments:   - char* result: result string
*              - bigint** a: bigint operand1
*              - bigint** b: bigint operand2
*              - bigint** c: bigint result
*              - char operate: operator
**************************************************/
void operate_string_cat(OUT char* result, IN bigint** a, IN bigint** b, IN bigint** c, IN char operate){
    int idx = 0;
    idx += bigint_to_hex(a, &result[idx]);
    idx += sprintf(&result[idx], " %c ", operate);
    idx += bigint_to_hex(b, &result[idx]);
    idx += sprintf(&result[idx], " = ");
    idx += bigint_to_hex(c, &result[idx]);
    result[idx] = '\0';
}




















