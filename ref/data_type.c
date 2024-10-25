#include "data_type.h"

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

void int_to_char(word num, char *str, int idx){
    while (num != 0){
        str[idx] = num % 10 + '0';
        num /= 10;
    }
}