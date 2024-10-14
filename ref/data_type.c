#include "data_type.h"

/*************************************************
* Name:        string_to_int
*
* Description: Convert string to integer
*
* Arguments:   - char* int_str: string of bigint
*              - word* result: result of integer
**************************************************/
dword string_to_int(char* str, int base){
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
int char_to_int(char str){
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
void int_to_char(word num, char *str, int idx){
    while (num != 0){
        str[idx] = num % 10 + '0';
        num /= 10;
    }
}