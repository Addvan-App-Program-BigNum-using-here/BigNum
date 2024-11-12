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

    if(*src == NULL)    return -1;
    idx += ((*src)->sign == 1) ? snprintf(&str[idx], 2, "-") : 0; // 부호 값 저장
    idx += snprintf(&str[idx], 3, "0x");
    for (int i = (*src)->word_len - 1; i >= 0; i--) {
        idx += snprintf(&str[idx], 9, "%08X", (*src)->a[i]);
    }
    return idx;
}


///*************************************************
//* Name:        String_refine
//*
//* Description: Remove leading zeros from string
//*
//* Arguments:   - const char* str: string
//**************************************************/
//char* String_refine(const IN char* str) {
//    if (str == NULL || str[0] == '\0')  return NULL;
//    int allZero = 1;
//    for (int i = 0; str[i] != '\0'; i++) {
//        if (str[i] != '0') {
//            allZero = 0;
//            break;
//        }
//    }
//    if (allZero) {
//        char* result = (char*)malloc(2);  // "0" + null terminator
//        if (result) {
//            result[0] = '0';
//            result[1] = '\0';
//        }
//        return result;
//    }
//
//    int startIndex = 0;
//    while (binary[startIndex] == '0') {
//        startIndex++;
//    }
//
//    int newLength = strlen(binary) - startIndex;
//    char* result = (char*)malloc(newLength + 1);
//    if (result == NULL) return NULL;
//
//    strncpy(result, str + startIndex, newLength + 1);
//    return result;
//}


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
msg operate_string_cat(OUT char* result, IN bigint** a, IN bigint** b, IN bigint** c, IN char operate){
    int idx = 0;
    int result_idx = 0;

    result_idx = bigint_to_hex(a, &result[idx]);
    if(idx == -1)    return -1;
    idx += result_idx;

    result_idx = snprintf(&result[idx], 4, " %c ", operate);
    if(idx == -1)    return -1;
    idx += result_idx;

    result_idx = bigint_to_hex(b, &result[idx]);
    if(idx == -1)    return -1;
    idx += result_idx;

    result_idx = snprintf(&result[idx], 4, " = ");
    if(idx == -1)    return -1;
    idx += result_idx;

    result_idx = bigint_to_hex(c, &result[idx]);
    if(idx == -1)    return -1;
    idx += result_idx;

    return 1;
}