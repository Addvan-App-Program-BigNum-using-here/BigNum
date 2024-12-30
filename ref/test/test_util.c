#include "test_util.h"

msg new_bigint_test_vector(){
    msg result_msg = new_bigint_test_vector_FAIL;
    char* str = NULL;
    bigint* a = NULL;

    // file clear
    if(Test_file_write_non_enter(Test_bigint, "", CLEAR) != FILE_WRITE_SUCCESS)   return new_bigint_test_vector_FAIL;

    str = (char *)calloc((test_word_size * 8) * 4 + 100, sizeof(char));
    if (str == NULL)    return MEM_NOT_ALLOC;

    for(int i = 0; i < test_size; i++){
        for(int j = 0; j < 3; j++){
            result_msg = get_random_bigint(RANDOM_TEST_WORD_SIZE, test_word_size, test_word_size_limit, 1, &a);
            if(result_msg != BI_GET_RANDOM_SUCCESS)    goto NEW_BIGINT_EXIT;
            bigint_to_hex(str, &a);
            Test_file_write_non_enter(Test_bigint, str, APPEND);
            Test_file_write_non_enter(Test_bigint, "\n", APPEND);
        }
    }
    result_msg = new_bigint_test_vector_SUCCESS;
NEW_BIGINT_EXIT:
    if (str != NULL)    free(str);
    if (a != NULL)    bi_delete(&a);
    return result_msg;
}

void printProgressBar(float percentage, int width){
    const char* blocks[] = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};
    int pos = (int)((percentage / 100.0f) * width * 8);
    int full_blocks = pos / 8;
    int remainder = pos % 8;

    printf("\r[");

    for (int i = 0; i < full_blocks; i++) {
        printf("%s", blocks[8]);
    }

    if (full_blocks < width) {
        printf("%s", blocks[remainder]);

        for (int i = full_blocks + 1; i < width; i++) {
            printf("%s", blocks[0]);
        }
    }

    printf("] %.1f%%", percentage);
    fflush(stdout);
}

void print_data_set(){
    printf("\n--------------- WORD INFO ---------------");
    printf("\nBigint bit size : %d", BITS_SIZE);
    printf("\nWord Bit Size : %d, Word size : %d", WORD_BITS, test_word_size);
    printf("\nitersize : %d", test_size);
    printf("\n-----------------------------------------\n");
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