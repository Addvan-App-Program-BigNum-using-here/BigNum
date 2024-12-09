#include "test_main.h"

#include <stdio.h>
#include <stdlib.h>
int main() {
    FILE *fp = NULL;
    // Python 스크립트 실행
    fp = _popen("python ../../sage_test/test.py", "r"); // 출력 리다이렉션 제거
    if (fp == NULL) {
        printf("Failed to run command. Check Python path and script location.\n");
        return 0;
    }
    // 출력 읽기 및 디버깅
    char buffer[128];
    printf("Python script output:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    // 명령 스트림 닫기
    int status = _pclose(fp);
    if (status == -1) {
        perror("_pclose failed");
        return 0;
    } else {
        printf("Python script exited with status %d\n", status);
    }
    return 0;
}



msg CLEAR_Test_file(){
    if(Test_file_write_non_enter(Test_file_gcd, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_RSA, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_RSA_CRT, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_EEA, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_MR, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_add, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_sub, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_mul, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_mul_karachuba, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_div, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_squ, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_squ_karachuba, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_exp, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;
    if(Test_file_write_non_enter(Test_file_barrett_reduction, "", CLEAR) != FILE_WRITE_SUCCESS)   return CLEAR_Test_file_FAIL;

    return CLEAR_Test_file_SUCCESS;
}

void print_data_set(){
    printf("\n--------------- WORD INFO ---------------");
    printf("\nBigint bit size : %d", BITS_SIZE);
    printf("\nWord Bit Size : %d, Word size : %d", WORD_BITS, test_word_size);
    printf("\nitersize : %d", test_size);
    printf("\n-----------------------------------------\n");
}
