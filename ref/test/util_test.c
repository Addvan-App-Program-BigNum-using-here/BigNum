#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_util.h"

#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif

struct timeval start, end;
double time_used;
clock_t c_start, c_end;

msg test_bi_get_lower();
msg test_bi_cat();

int main(){
    if(test_bi_get_lower() != Test_BI_GET_LOWER_SUCCESS)    return Test_FAIL;
    if(test_bi_cat() != Test_BI_CAT_SUCCESS)    return Test_FAIL;
    return Test_SUCCESS;
}

msg test_bi_get_lower(){
    int rand_test_word_size = test_word_size;
    msg result_msg = Test_BI_GET_LOWER_SUCCESS;
    bigint *a = NULL;
    bigint *b = NULL;
    char* str = NULL;
    char* lower_bit_str = NULL;
    byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
    int lower_bit_size = 0;
    double total_time_get_lower = 0;
    ParamType param_types[2] = {TYPE_BIGINT_PTR, TYPE_INT_PTR};

    printf("\n============ Testing bi_get_lower ============\n");

    if(test_word_size > 0){
        if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
        lower_bit_size = temp[0] % (rand_test_word_size * WORD_BITS);
        lower_bit_str = int_to_string(lower_bit_size);
    }

    if (Test_file_write(Test_main_file, GET_LOWER_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for(int i = 0; i < test_size; i++){
        if(test_word_size <= 0){
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                rand_test_word_size = temp[0] % test_word_size_limit;
                // lower size
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                lower_bit_size = temp[0] % (rand_test_word_size * WORD_BITS);
                lower_bit_str = int_to_string(lower_bit_size);

            }while(rand_test_word_size <= 0);
        }

        str = (char*)calloc(rand_test_word_size * WORD_BITS + 1, sizeof(char));
        if (str == NULL)    return MEM_NOT_ALLOC;

        // 랜덤한 bigint 생성
        result_msg = bi_get_random(&a, rand_test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto GET_LOWER_EXIT_FREE;
        else if(a->word_len != rand_test_word_size){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto GET_LOWER_EXIT_FREE;
        }
        a->sign = 0;

        if (bigint_to_hex(str, &a) == -1)   goto GET_LOWER_EXIT_FREE;
        result_msg = Test_file_write_non_enter(Test_main_file, str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto GET_LOWER_EXIT_FREE;

        // get_lower test
        result_msg = Test_file_write_non_enter(Test_main_file, " & ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto GET_LOWER_EXIT_FREE;

        // lower size 저장
        result_msg = Test_file_write_non_enter(Test_main_file, lower_bit_str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto GET_LOWER_EXIT_FREE;

        // get_lower 수행
        total_time_get_lower += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_get_lower, &b, &result_msg, param_types, &a, &lower_bit_size);
        if (result_msg != BI_GET_LOWER_SUCCESS)   goto GET_LOWER_EXIT_FREE;

        result_msg = Test_file_write_non_enter(Test_main_file, " = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto GET_LOWER_EXIT_FREE;

        // 결과 값 저장
        if (bigint_to_hex(str, &b) == -1)   goto GET_LOWER_EXIT_FREE;
        result_msg = Test_file_write(Test_main_file, str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto GET_LOWER_EXIT_FREE;

        free(str);
        if(test_word_size <= 0) free(lower_bit_str);
    }
    result_msg = Test_BI_GET_LOWER_SUCCESS;
    goto GET_LOWER_EXIT;

GET_LOWER_EXIT_FREE:
    free(str);
    free(lower_bit_str);
GET_LOWER_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (result_msg != Test_BI_GET_LOWER_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", total_time_get_lower / test_size);
    log_msg(result_msg);
    if (Test_file_write(Test_main_file, seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_cat(){
    int rand_test_word_size = test_word_size;
    msg result_msg = Test_BI_CAT_SUCCESS;
    bigint* a = NULL;
    bigint* b = NULL;
    bigint* c = NULL;
    char* str = NULL;
    int a_len = 0;
    int b_len = 0;
    byte temp[1] = {0};
    double total_time_cat = 0;
    ParamType param_types[2] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};

    printf("\n============ Testing cat ============\n");

    if(test_word_size < 3){
        printf("random test size is too small\n");
        return Test_BI_CAT_SUCCESS;
    }

    if(test_word_size > 0){
        do{
            if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
            a_len = temp[0] % rand_test_word_size;
            b_len = rand_test_word_size - a_len;
        }while(a_len <= 0 || b_len <= 0);
    }

    if (Test_file_write(Test_main_file, CAT_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for(int i = 0; i < test_size; i++){
        if(test_word_size <= 0){
            // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                rand_test_word_size = temp[0] % test_word_size_limit;

                // bigint a, b size
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                a_len = temp[0] % rand_test_word_size;
                b_len = rand_test_word_size - a_len;
            }while(rand_test_word_size <= 0 || a_len == 0 || b_len == 0);
        }

        str = (char*)calloc(rand_test_word_size * WORD_BITS + 1, sizeof(char));
        if (str == NULL)    return MEM_NOT_ALLOC;

        // 랜덤한 bigint 생성
        result_msg = bi_get_random(&a, a_len);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto CAT_EXIT_FREE;
        else if(a->word_len != a_len){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto CAT_EXIT_FREE;
        }
        a->sign = 0;

        if (bigint_to_hex(str, &a) == -1)   goto CAT_EXIT_FREE;
        result_msg = Test_file_write_non_enter(Test_main_file, str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto CAT_EXIT_FREE;
        // get_lower test
        result_msg = Test_file_write_non_enter(Test_main_file, " || ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto CAT_EXIT_FREE;

        // 랜덤한 bigint 생성
        result_msg = bi_get_random(&b, b_len);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto CAT_EXIT_FREE;
        else if(b->word_len != b_len){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto CAT_EXIT_FREE;
        }
        b->sign = 0;

        if (bigint_to_hex(str, &b) == -1)   goto CAT_EXIT_FREE;
        result_msg = Test_file_write_non_enter(Test_main_file, str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto CAT_EXIT_FREE;

        // bi_cat 수행
        total_time_cat += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_cat, &c, &result_msg, param_types, &a, &b);
        if (result_msg != BI_CAT_SUCCESS)   goto CAT_EXIT_FREE;

        result_msg = Test_file_write_non_enter(Test_main_file, " = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto CAT_EXIT_FREE;

        // 결과 값 저장
        if (bigint_to_hex(str, &c) == -1)   goto CAT_EXIT_FREE;
        result_msg = Test_file_write(Test_main_file, str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto CAT_EXIT_FREE;

        free(str);
    }
    result_msg = Test_BI_CAT_SUCCESS;
    goto CAT_EXIT;

CAT_EXIT_FREE:
    free(str);
CAT_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (result_msg != Test_BI_CAT_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", total_time_cat / test_size);
    log_msg(result_msg);
    if (Test_file_write(Test_main_file, seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}