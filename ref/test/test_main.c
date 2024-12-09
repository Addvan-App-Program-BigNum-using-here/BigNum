#include "test_main.h"

int main(){
    FILE *fp = NULL;

    // 테스트 파일 초기화
    CLEAR_Test_file();

    // 고정 입력 값에 대한 성능 평가
    if(cmp_operate_test() != Test_SUCCESS)    return 0;
    if(cmp_crypto_test() != Test_SUCCESS)    return 0;

    // 랜덤 입력 값에 대한 성능 평가
//    if(rand_operate_test() != Test_SUCCESS)    return 0;
//    if(rand_crypto_test() != Test_SUCCESS)    return 0;

    // 곱셈, 나눗셈, 제곱 연산에 대한 성능 평가
//    if(func_compare() != Test_SUCCESS)    return 0;

    // Sage test
    fp = popen("python3 ../../sage_test/test.py >/dev/null 2>&1", "r");
    if (fp == NULL){
        printf("Failed to run command\n");
        return 0;
    }

    // Close the command stream
    int status = pclose(fp);
    if (status == -1){
        perror("pclose failed");
        return 0;
    }
    else
        printf("python script exited with status %d\n", status);

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
