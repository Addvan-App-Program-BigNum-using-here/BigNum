#include "util_test.h"

struct timeval start, end;
double time_used;
clock_t c_start, c_end;

int main(){
    FILE *fp = NULL;
    double op_total_time[12] = {0, };
    double op_exp_time[3] = {0, };
    byte temp[1] = {0};
    int test_word_size_a = test_word_size;
    int test_word_size_b = test_word_size;
    int test_word_size_c = test_word_size;
    int test_max_word_size = test_word_size;
    int karachuba_flag = 0;
    int squ_karachuba_flag = 0;
    int DIVISION_METHOD = 0;
    int miller_rabin_iter = 10;
    char* str = NULL;
    msg result_msg = Test_SUCCESS;
    bigint* a = NULL;
    bigint* b = NULL;
    bigint* c = NULL;
    bigint* barret_N = NULL;
    bigint* barret_T = NULL;

    for(int i = 0; i < 10; i++){
        printf("%dth test\n", i);
//        result_msg = RSA();
        result_msg = RSA_CRT();
        log_msg(result_msg);
    }
    return 0;
/*
//    test_bi_new_delete();       // bigint 할당 및 해제 테스트
//    test_bi_random();           // 랜덤 bigint 생성 테스트
//    test_bi_set_from();         // string으로부터 bigint 생성 테스트
//    test_bi_shift();            // bigint shift 테스트
//    test_bi_get_lower();        // bigint modular 테스트
//    test_bi_cat();              // bigint cat 테스트

    // barret N 생성
    result_msg = init_barret_N(&barret_T, &barret_N, barret_word_size);
    if (result_msg != INIT_BARRET_N_SUCCESS){
        log_msg(result_msg);
        return Test_FAIL;
    }

    // 카라츄바 세팅
    if(init_karachuba_pool(test_word_size) != INIT_KARACHUBA_POOL_SUCCESS){
        log_msg(INIT_KARACHUBA_POOL_FAIL);
        return Test_FAIL;
    }

    for(int i = 0; i < test_size; i++){
        // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당
        if(test_word_size <= 0){
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_a = temp[0] % test_word_size_limit;
            }while(test_word_size_a <= 0);
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_b = temp[0] % test_word_size_limit;
            }while(test_word_size_b <= 0);
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_c = temp[0] % test_word_size_limit;
            }while(test_word_size_c <= 0);
            test_max_word_size = max(test_word_size_a, test_word_size_b);
            test_max_word_size = max(test_max_word_size, test_word_size_c);
        }

        // 랜덤으로 a, b, c 생성
        result_msg = bi_get_random(&a, test_word_size_a);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto TEST_EXIT;
        else if(a->word_len != test_word_size_a){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto TEST_EXIT;
        }

        result_msg = bi_get_random(&b, test_word_size_b);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto TEST_EXIT;
        else if(b->word_len != test_word_size_b){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto TEST_EXIT;
        }

        result_msg = bi_get_random(&c, test_word_size_c);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto TEST_EXIT;
        else if(b->word_len != test_word_size_b){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto TEST_EXIT;
        }

        // 문자열 담을 배열 선언 크기는 다시 고려
        str = (char *)calloc((test_max_word_size * 8) * 4 + 100, sizeof(char)); // '0x' * 3 + 부호 * 3 + " + " + " = " => 6 + 3 + 3 + 3 = 15
        if (str == NULL){
            result_msg = MEM_NOT_ALLOC;
            goto TEST_EXIT;
        }

//        // bigint 덧셈 테스트
//        result_msg = test_bi_add(&op_total_time[0], &a, &b, str);
//        if(result_msg != Test_BI_ADD_SUCCESS){
//            log_msg(Test_BI_ADD_FAIL);
//            log_msg(result_msg);
//            return Test_FAIL;
//        }

        memset(str, 0, (test_max_word_size * 8) * 4 + 100); // str 초기화
        // bigint 뺄셈 테스트
        result_msg = test_bi_sub(&op_total_time[1], &a, &b, str);
        if(result_msg != Test_BI_SUB_SUCCESS){
            log_msg(Test_BI_SUB_FAIL);
            log_msg(result_msg);
            return Test_FAIL;
        }

        memset(str, 0, (test_max_word_size * 8) * 4 + 100); // str 초기화
        // BINARY LONG bigint 나눗셈 테스트
        DIVISION_METHOD = 0;
        result_msg = test_bi_div(&op_total_time[2], &a, &b, str, &DIVISION_METHOD);
        if(result_msg != Test_BI_DIV_SUCCESS){
            log_msg(Test_BI_DIV_FAIL);
            log_msg(result_msg);
            return Test_FAIL;
        }

        memset(str, 0, (test_max_word_size * 8) * 4 + 100); // str 초기화
        // WORD LONG bigint 나눗셈 테스트
        DIVISION_METHOD = 1;
        result_msg = test_bi_div(&op_total_time[3], &a, &b, str, &DIVISION_METHOD);
        if(result_msg != Test_BI_DIV_SUCCESS){
            log_msg(Test_BI_DIV_FAIL);
            log_msg(result_msg);
            return Test_FAIL;
        }

        memset(str, 0, (test_max_word_size * 8) * 4 + 100); // str 초기화
        // bigint 곱셈 테스트
        result_msg = test_bi_mul(&op_total_time[4], &a, &b, str);
        if(result_msg != Test_BI_MUL_SUCCESS){
            log_msg(Test_BI_MUL_FAIL);
            log_msg(result_msg);
            return Test_FAIL;
        }

        memset(str, 0, (test_max_word_size * 8) * 4 + 100); // str 초기화
        // bigint 카라츄바 곱셈 테스트
        karachuba_flag = test_max_word_size / mul_karachuba_ratio;
        result_msg = test_bi_mul_karachuba(&op_total_time[5], &a, &b, str, &karachuba_flag);
        if(result_msg != Test_BI_MUL_KARACHUBA_SUCCESS){
            log_msg(Test_BI_MUL_KARACHUBA_FAIL);
            log_msg(result_msg);
            return Test_FAIL;
        }

        memset(str, 0, (test_max_word_size * 8) * 4 + 100); // str 초기화
        // bigint 제곱 테스트
        result_msg = test_bi_squ(&op_total_time[6], &a, str);
        if(result_msg != Test_BI_SQU_SUCCESS){
            log_msg(Test_BI_SQU_FAIL);
            log_msg(result_msg);
            return Test_FAIL;
        }

        squ_karachuba_flag = test_max_word_size / squ_karachuba_ratio;
        memset(str, 0, (test_max_word_size * 8) * 4 + 100); // str 초기화
        // bigint 카라츄바 제곱 테스트
        result_msg = test_bi_squ_karachuba(&op_total_time[7], &a, str, &squ_karachuba_flag);
        if(result_msg != Test_BI_SQU_KARACHUBA_SUCCESS){
            log_msg(Test_BI_SQU_KARACHUBA_FAIL);
            log_msg(result_msg);
            return Test_FAIL;
        }

        memset(str, 0, (test_max_word_size * 8) * 4 + 100); // str 초기화
        // bigint 지수승 테스트
        result_msg = test_bi_exp(op_exp_time, &a, &b, &c, str);
        if(result_msg != Test_BI_EXP_SUCCESS){
            return Test_FAIL;
        }

        if(test_word_size == barret_word_size){ // 사전 연산 값이 고정되어 있기에 test_word_size가 기존 사이즈와 같을 때만 수행
            memset(str, 0, (test_word_size * 8) * 4 + 100); // str 초기화
            // bigint Barrett Reduction 테스트
            result_msg = test_bi_barrett_reduction(&op_total_time[8], &a, &barret_N, &barret_T, str);
            if(result_msg != Test_BI_BARRETT_REDUCTION_SUCCESS){
                log_msg(Test_BI_BARRETT_REDUCTION_FAIL);
                log_msg(result_msg);
                return Test_FAIL;
            }
        }

        memset(str, 0, (test_max_word_size * 8) * 4 + 100); // str 초기화
        // bigint gcd 테스트
        result_msg = test_bi_gcd(&op_total_time[9], &a, &b, str);
        if(result_msg != Test_BI_GCD_SUCCESS){
            log_msg(Test_BI_GCD_FAIL);
            log_msg(result_msg);
            return Test_FAIL;
        }

        // bigint EEA 테스트
        a->sign = 0;
        b->sign = 0;
        memset(str, 0, (test_max_word_size * 8) * 4 + 100); // str 초기화
        // bigint EEA 테스트
        result_msg = test_bi_EEA(&op_total_time[10], &a, &b, str);
        if(result_msg != Test_BI_EEA_SUCCESS){
            log_msg(Test_BI_EEA_FAIL);
            log_msg(result_msg);
            return Test_FAIL;
        }


        printf("%dth test\n", i);
        a->sign = 0;
        memset(str, 0, (test_max_word_size * 8) * 4 + 100); // str 초기화
        result_msg = test_miller_rabin(&op_total_time[11], &a, &miller_rabin_iter, str);
        if(result_msg != Test_MILLER_RABIN_SUCCESS){
            log_msg(Test_MILLER_RABIN_FAIL);
            log_msg(result_msg);
            return Test_FAIL;
        }

    }

//    printf("\n--------------- WORD INFO ---------------");
//    printf("\nBigint bit size : %d", BITS_SIZE);
//    printf("\nWord Bit Size : %d, Word size : %d", WORD_BITS, test_word_size);
//    printf("\nitersize : %d", test_size);
//    printf("\n============ Testing bi_add ============\n");
//    printf("Time taken add : %f seconds\n", op_total_time[0] / test_size);

    printf("\n--------------- WORD INFO ---------------");
    printf("\nBigint bit size : %d", BITS_SIZE);
    printf("\nWord Bit Size : %d, Word size : %d", WORD_BITS, test_word_size);
    printf("\nitersize : %d", test_size);
    printf("\n============ Testing bi_sub ============\n");
    printf("Time taken sub : %f seconds\n", op_total_time[1] / test_size);

    printf("\n--------------- WORD INFO ---------------");
    printf("\nBigint bit size : %d", BITS_SIZE);
    printf("\nWord Bit Size : %d, Word size : %d", WORD_BITS, test_word_size);
    printf("\nitersize : %d", test_size);
    printf("\n============ Testing bi_div (Binary Long Division) ============\n");
    printf("Time taken div(Binary) : %f seconds\n", op_total_time[2] / test_size);

    printf("\n--------------- WORD INFO ---------------");
    printf("\nBigint bit size : %d", BITS_SIZE);
    printf("\nWord Bit Size : %d, Word size : %d", WORD_BITS, test_word_size);
    printf("\nitersize : %d", test_size);
    printf("\n============ Testing bi_div (WORD Long Division) ============\n");
    printf("Time taken div(Long) : %f seconds\n", op_total_time[3] / test_size);

    printf("\n--------------- WORD INFO ---------------");
    printf("\nBigint bit size : %d", BITS_SIZE);
    printf("\nWord Bit Size : %d, Word size : %d", WORD_BITS, test_word_size);
    printf("\nitersize : %d", test_size);
    printf("\n============ Testing bi_mul ============\n");
    printf("Time taken mul : %f seconds\n", op_total_time[4] / test_size);

    printf("\n--------------- WORD INFO ---------------");
    printf("\nBigint bit size : %d", BITS_SIZE);
    printf("\nWord Bit Size : %d, Word size : %d", WORD_BITS, test_word_size);
    printf("\nitersize : %d", test_size);
    printf("\n============ Testing bi_mul_karachuba ============\n");
    printf("Time taken mul_karachuba : %f seconds\n", op_total_time[5] / test_size);

    printf("\n--------------- WORD INFO ---------------");
    printf("\nBigint bit size : %d", BITS_SIZE);
    printf("\nWord Bit Size : %d, Word size : %d", WORD_BITS, test_word_size);
    printf("\nitersize : %d", test_size);
    printf("\n============ Testing bi_squ ============\n");
    printf("Time taken squ : %f seconds\n", op_total_time[6] / test_size);

    printf("\n--------------- WORD INFO ---------------");
    printf("\nBigint bit size : %d", BITS_SIZE);
    printf("\nWord Bit Size : %d, Word size : %d", WORD_BITS, test_word_size);
    printf("\nitersize : %d", test_size);
    printf("\n============ Testing bi_squ_karachuba ============\n");
    printf("Time taken squ_karachuba : %f seconds\n", op_total_time[7] / test_size);


    printf("\n============ Testing bi_exp ============\n");
    printf("Time taken exp (MS) : %f seconds\n", op_exp_time[0] / test_size);
    printf("Time taken exp (R TO L) : %f seconds\n", op_exp_time[1] / test_size);
    printf("Time taken exp (L TO R) : %f seconds\n", op_exp_time[2] / test_size);

    printf("\n============ Testing bi_barrett_reduction ============\n");
    printf("Time taken barret_reduction : %f seconds\n", op_total_time[8] / test_size);

    printf("\n============ Testing bi_gcd ============\n");
    printf("Time taken gcd : %f seconds\n", op_total_time[10] / test_size);
    printf("\n");

//    if(compare_multiplicaiton(16, 120, 16) != COMPARE_MULTIPLICATION_SUCCESS)   return Test_FAIL;   // bigint 곱셈 성능 비교 테스트
//    if(compare_squaring(16, 120, 16) != COMPARE_SQUARING_SUCCESS)   return Test_FAIL;   // bigint 곱셈 성능 비교 테스트
//    if(compare_division(16, 120, 16) != COMPARE_DIVISION_SUCCESS)   return Test_FAIL;   // bigint 곱셈 성능 비교 테스트

    // 카라츄바 세팅 해제
    if(clear_karachuba_pool() != CLEAR_KARACHUBA_POOL_SUCCESS){
        log_msg(CLEAR_KARACHUBA_POOL_FAIL);
        return Test_FAIL;
    }

    printf("\n");
    log_msg(Test_SUCCESS);

    // Sage test
    fp = popen("python3 ../../sage_test/test.py >/dev/null 2>&1", "r");
    if (fp == NULL){
        printf("Failed to run command\n");
        return 1;
    }

    // Close the command stream
    int status = pclose(fp);
    if (status == -1){
        perror("pclose failed");
        return 1;
    }
    else
        printf("python script exited with status %d\n", status);

    return 0;
TEST_EXIT:
    if(bi_delete(&a) != BI_FREE_SUCCESS)    return Test_FAIL;
    if(bi_delete(&b) != BI_FREE_SUCCESS)    return Test_FAIL;
    if(bi_delete(&c) != BI_FREE_SUCCESS)    return Test_FAIL;

    log_msg(result_msg);
    return Test_FAIL;
    */
}

msg test_bi_new_delete(){
    bigint *a = NULL;
    int rand_test_word_size = test_word_size;
    msg result_msg = 0;
    double total_time_new = 0;
    double total_time_delete = 0;
    ParamType param_types[1] = {TYPE_INT_PTR};

    printf("\n============ Testing bi_new_delete ============\n");

    for (int i = 0; i < test_size; i++){
        if(test_word_size <= 0){
            do{
                byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
                result_msg = randombytes(temp, 1);
                if(result_msg != GEN_RANDOM_BYTES_SUCCESS)    goto EXIT_NEW_DEL;
                rand_test_word_size = temp[0] % test_word_size_limit;
            }while(rand_test_word_size <= 0);
        }

        total_time_new += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_new, &a, &result_msg, param_types, &rand_test_word_size);
        if(result_msg != BI_ALLOC_SUCCESS)  return BI_ALLOC_FAIL;

        if(a->word_len != rand_test_word_size){
            result_msg = bi_delete(&a);
            if(result_msg != BI_FREE_SUCCESS)    goto EXIT_NEW_DEL;
            return BI_ALLOC_FAIL;
        }
        c_start = clock();
        result_msg = bi_delete(&a);
        c_end = clock();
        total_time_delete += (double)(c_end - c_start) / CLOCKS_PER_SEC;
        if(result_msg != BI_FREE_SUCCESS)   goto EXIT_NEW_DEL;
    }
    result_msg = Test_BI_NEW_DELETE_SUCCESS;

EXIT_NEW_DEL:
    if (result_msg != Test_BI_NEW_DELETE_SUCCESS)   return Test_FAIL;
    printf("Time taken new : %f seconds\n", total_time_new / test_size);
    printf("Time taken delete : %f seconds\n", total_time_delete / test_size);
    log_msg(result_msg);
    if (Test_file_write(Test_main_file, seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_shift(){
    int rand_test_word_size = test_word_size;
    msg result_msg = Test_BI_SHIFT_SUCCESS;
    bigint *a = NULL;
    bigint *c = NULL;
    int shift_size = 0;
    double total_time_left_shift = 0;
    double total_time_right_shift = 0;
    byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
    char* shift_size_str = NULL;
    char* str = NULL;
    ParamType param_types[2] = {TYPE_BIGINT_PTR, TYPE_INT_PTR};

    printf("\n============ Testing bi_shfit ============\n");

    if (Test_file_write(Test_main_file, SHIFT_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for(int i = 0; i < test_size; i++){
        if(test_word_size <= 0){
            // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                rand_test_word_size = temp[0] % test_word_size_limit;
            }while(rand_test_word_size <= 0);
        }
       // shift size
        if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
        shift_size = temp[0] % (rand_test_word_size * WORD_BITS);
        shift_size_str = int_to_string(shift_size);

        str = (char*)calloc(2 * rand_test_word_size * WORD_BITS + 1, sizeof(char));
        if (str == NULL)    return MEM_NOT_ALLOC;

        // 랜덤한 bigint 생성
        result_msg = bi_get_random(&a, rand_test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto SHIFT_EXIT_FREE;
        else if(a->word_len != rand_test_word_size){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto SHIFT_EXIT_FREE;
        }

        a->sign = 0;

        if (bigint_to_hex(str, &a) == -1)   goto SHIFT_EXIT_FREE;
        result_msg = Test_file_write_non_enter(Test_main_file, str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // shift left test
        result_msg = Test_file_write_non_enter(Test_main_file, " << ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // shift size 저장
        result_msg = Test_file_write_non_enter(Test_main_file, shift_size_str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // left shift 수행
        total_time_left_shift += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_shift_left, &c, &result_msg, param_types, &a, &shift_size);
        if (result_msg != BI_SHIFT_SUCCESS)   goto SHIFT_EXIT_FREE;

        result_msg = Test_file_write_non_enter(Test_main_file, " = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // 결과 값 저장
        if (bigint_to_hex(str, &c) == -1)   goto SHIFT_EXIT_FREE;
        result_msg = Test_file_write(Test_main_file, str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;


        // shift right test
        if (bigint_to_hex(str, &a) == -1)   goto SHIFT_EXIT_FREE;
        result_msg = Test_file_write_non_enter(Test_main_file, str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // shift left test
        result_msg = Test_file_write_non_enter(Test_main_file, " >> ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // shift size 저장
        result_msg = Test_file_write_non_enter(Test_main_file, shift_size_str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // right shift 수행
        total_time_right_shift += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_shift_right, &c, &result_msg, param_types, &a, &shift_size);\
        if (result_msg != BI_SHIFT_SUCCESS)   goto SHIFT_EXIT_FREE;

        result_msg = Test_file_write_non_enter(Test_main_file, " = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // 결과 값 저장
        if (bigint_to_hex(str, &c) == -1)   goto SHIFT_EXIT_FREE;
        result_msg = Test_file_write(Test_main_file, str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        free(str);
        free(shift_size_str);
    }
    result_msg = Test_BI_SHIFT_SUCCESS;
    goto SHIFT_EXIT;

SHIFT_EXIT_FREE:
    free(str);
    free(shift_size_str);
SHIFT_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (result_msg != Test_BI_SHIFT_SUCCESS)   return Test_FAIL;
    printf("Time taken left shift : %f seconds\n", total_time_left_shift / test_size);
    printf("Time taken right shift : %f seconds\n", total_time_right_shift / test_size);
    log_msg(result_msg);
    if (Test_file_write(Test_main_file, seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
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

msg test_bi_set_from(){
    bigint *a = NULL;
    word *test_array = NULL;
    int rand_test_word_size = test_word_size;
    double total_set_from_array_little = 0;
    double total_set_from_array_big = 0;
    ParamType param_types[3] = {TYPE_INT_PTR, TYPE_WORD_ARR_PTR, TYPE_INT_PTR};
    msg result_msg = Test_BI_SET_FROM_FAIL;
    int base = 2;

    printf("\n============ Testing bi_set_from ============\n");

    if (Test_file_write(Test_main_file, FROM_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    // 문자열로 들어온 hex를 endian에 맞추어 잘 저장하는지 테스트
    for (int i = 0; i < test_size; i++){
        // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
        if(test_word_size <= 0){
            // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
            do{
                byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                rand_test_word_size = temp[0] % (test_word_size_limit * 10);
            }while(rand_test_word_size <= 0);
        }
        // 테스트 할 배열 생성
        test_array = (word *)calloc(rand_test_word_size, sizeof(word));
        if (test_array == NULL)
            return MEM_NOT_ALLOC;

        // 랜덤한 배열 생성
        result_msg = array_random(test_array, rand_test_word_size);
        if (result_msg != GEN_RANDOM_SUCCESS)   goto FROM_EXIT_TEST_ARRAY;

        int little_endian = 1;
        // little endian set test
        total_set_from_array_little += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_set_from_array, &a, &result_msg, param_types, &rand_test_word_size, test_array, &little_endian);
        if (result_msg != BI_SET_ARRAY_SUCCESS) goto FROM_EXIT_TEST_ARRAY;

        // 할당이 잘 되었는지 확인
        for (int i = 0; i < rand_test_word_size; i++){
            if (a->a[i] != test_array[i]){
                result_msg = BI_SET_ARRAY_NOT_MATCH;
                goto FROM_EXIT_TEST_ARRAY;
            }
        }

        int big_endian = 0;
        // big_endian set test
        total_set_from_array_big += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_set_from_array, &a, &result_msg, param_types, &rand_test_word_size, test_array, &big_endian);
        if (result_msg != BI_SET_ARRAY_SUCCESS) goto FROM_EXIT_TEST_ARRAY;

        for (int i = 0; i < rand_test_word_size; i++){
            if (a->a[(rand_test_word_size - 1) - i] != test_array[i]){
                result_msg = BI_SET_ARRAY_NOT_MATCH;
                goto FROM_EXIT_TEST_ARRAY;
            }
        }
    }

    // 2진수 테스트
    result_msg = Test_file_write(Test_main_file, "[2]", APPEND);
    if(result_msg != FILE_WRITE_SUCCESS)    goto FROM_EXIT;
    result_msg = test_bi_set_from_base(&base);
    if(result_msg != Test_BI_SET_FROM_BASE_SUCCESS)    goto FROM_EXIT;

    result_msg = Test_file_write(Test_main_file, seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)   goto FROM_EXIT;

    // 10진수 테스트
    result_msg = Test_file_write(Test_main_file, "[10]", APPEND);
//    if(result_msg != FILE_WRITE_SUCCESS)    goto FROM_EXIT;
//    result_msg = test_bi_set_from_base(10);
//    if(result_msg != Test_BI_SET_FROM_BASE_SUCCESS)    goto FROM_EXIT;

    result_msg = Test_file_write(Test_main_file, seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)   goto FROM_EXIT;

    base = 16;
    // 16진수 테스트
    result_msg = Test_file_write(Test_main_file, "[16]", APPEND);
    if(result_msg != FILE_WRITE_SUCCESS)    goto FROM_EXIT;;
    result_msg = test_bi_set_from_base(&base);
    if(result_msg != Test_BI_SET_FROM_BASE_SUCCESS)    goto FROM_EXIT;;

    result_msg = Test_file_write(Test_main_file, seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)   goto FROM_EXIT;

    result_msg = Test_BI_SET_FROM_SUCCESS;
    goto FROM_EXIT;

FROM_EXIT_TEST_ARRAY:
    free(test_array);
FROM_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   result_msg = BI_FREE_FAIL;
    if (result_msg != Test_BI_SET_FROM_SUCCESS)   return Test_FAIL;
    printf("Time taken from array(little): %f seconds\n", total_set_from_array_little / test_size);
    printf("Time taken from array(big): %f seconds\n", total_set_from_array_big / test_size);
    log_msg(result_msg);
    if (Test_file_write(Test_main_file, seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_set_from_base(IN int* base){
    bigint *a = NULL;
    char *str = NULL;
    char *str_base = NULL;
    msg result_msg = 0;
    int array_size = test_word_size; // 최대 4바이트 크기의 정수
    double total_set_from_base = 0;
    ParamType param_types[2] = {TYPE_STR_PTR, TYPE_INT_PTR};

    for (int i = 0; i < test_size; i++){
        if(test_word_size <= 0){
            // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
            do{
                byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                array_size = temp[0] % (test_word_size_limit * 10);
            }while(array_size <= 0);
        }

        // 테스트 할 배열 생성
        str = (char*)calloc(array_size * 32 + 20, sizeof(byte)); // '0x' * 3 + '-' * 3 + " + " + " = " => 6 + 3 + 3 + 3 = 14
        if(str == NULL) return MEM_NOT_ALLOC;

        // base 기준 랜덤한 str 가져오기
        result_msg = get_random_string(&str_base, array_size, *base); // 오버플로우 나면 여기 먼저 수정
        if(result_msg != RAND_STRING_SUCCESS) goto FROM_BASE_EXIT_FREE;

        // 랜덤한 문자열 저장
        result_msg = Test_file_write(Test_main_file, str_base, APPEND); // 랜덤한 문자열 저장
        if (result_msg != FILE_WRITE_SUCCESS)   goto FROM_BASE_EXIT_FREE;

        // base 테스트
        total_set_from_base += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_set_from_string, &a, &result_msg, param_types, str_base, base);
        if (result_msg != BI_SET_STRING_SUCCESS)    goto FROM_BASE_EXIT_FREE;

        // bigint to hex string
        if (bigint_to_hex(str, &a) == -1)   goto FROM_BASE_EXIT_FREE;

        // 변환한 16진수 문자열을 txt에 넣는다.
        result_msg = Test_file_write(Test_main_file, str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto FROM_BASE_EXIT_FREE;

        free(str);
    }
    result_msg = Test_BI_SET_FROM_BASE_SUCCESS;
    goto FROM_BASE_EXIT;

FROM_BASE_EXIT_FREE:
    free(str);
FROM_BASE_EXIT:
    free(str_base);
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    printf("Time taken from string %d digit: %f seconds\n", *base, total_set_from_base / test_size);
    return result_msg;
}

msg test_bi_random(){
    bigint *dst = NULL;
    msg result_msg = 0;
    int rand_test_word_size = test_word_size;
    double total_time_random = 0;
    ParamType param_types[1] = {TYPE_INT_PTR};

    printf("\n============ Testing bi_random ============\n");

    for (int i = 0; i < test_size; i++){
        // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
        if(test_word_size <= 0){
            do{
                byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
                result_msg = randombytes(temp, 1);
                if(result_msg != GEN_RANDOM_BYTES_SUCCESS)    goto EXIT_RANDOM;
                rand_test_word_size = temp[0] % test_word_size_limit;
            }while(rand_test_word_size <= 0);
        }

        total_time_random += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_get_random, &dst, &result_msg, param_types, &rand_test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS){
            result_msg = bi_delete(&dst);
            if (result_msg != BI_FREE_SUCCESS) goto EXIT_RANDOM;
            result_msg = Test_BI_GET_RANDOM_FAIL;
            goto EXIT_RANDOM;
        }
    }
    result_msg = bi_delete(&dst);
    if (result_msg != BI_FREE_SUCCESS)  goto EXIT_RANDOM;

    result_msg = Test_BI_GET_RANDOM_SUCCESS;

EXIT_RANDOM:
    if (result_msg != Test_BI_GET_RANDOM_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", total_time_random / test_size);
    log_msg(result_msg);
    if (Test_file_write(Test_main_file, seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_add(OUT double* total_time_add, IN bigint** a, IN bigint** b, IN char* str){
    bigint *c = NULL;
    msg result_msg = Test_BI_ADD_SUCCESS;
    ParamType param_types[2] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};

    if (bigint_to_hex(str, a) == -1)   goto ADD_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_add, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto ADD_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_add, " + ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto ADD_EXIT;

    if (bigint_to_hex(str, b) == -1)   goto ADD_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_add, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto ADD_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_add, " = ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto ADD_EXIT;

    *total_time_add += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_add, &c, &result_msg, param_types, a, b);
    if (result_msg != BI_ADD_SUCCESS)   goto ADD_EXIT;

    if (bigint_to_hex(str, &c) == -1)   goto ADD_EXIT;
    result_msg = Test_file_write(Test_file_add, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto ADD_EXIT;

    result_msg = Test_BI_ADD_SUCCESS;

ADD_EXIT:
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_sub(OUT double* total_time_sub, IN bigint** a, IN bigint** b, IN char* str){
    bigint *c = NULL;
    msg result_msg = Test_BI_SUB_SUCCESS;
    ParamType param_types[2] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};

    if (bigint_to_hex(str, a) == -1)   goto SUB_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_sub, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SUB_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_sub, " - ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SUB_EXIT;

    if (bigint_to_hex(str, b) == -1)   goto SUB_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_sub, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SUB_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_sub, " = ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SUB_EXIT;

    *total_time_sub += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_sub, &c, &result_msg, param_types, a, b);
    if (result_msg != BI_SUB_SUCCESS)   goto SUB_EXIT;

    if (bigint_to_hex(str, &c) == -1)   goto SUB_EXIT;
    result_msg = Test_file_write(Test_file_sub, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SUB_EXIT;

    result_msg = Test_BI_SUB_SUCCESS;

SUB_EXIT:
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_mul(OUT double* total_time_mul, IN bigint** a, IN bigint** b, IN char* str){
    bigint *c = NULL;
    msg result_msg = Test_BI_MUL_SUCCESS;
    ParamType param_types[2] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};

    if (bigint_to_hex(str, a) == -1)   goto MUL_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_mul, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_mul, " * ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_EXIT;

    if (bigint_to_hex(str, b) == -1)   goto MUL_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_mul, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_mul, " = ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_EXIT;

    *total_time_mul += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_mul, &c, &result_msg, param_types, a, b);
    if (result_msg != BI_MUL_SUCCESS)   goto MUL_EXIT;

    if (bigint_to_hex(str, &c) == -1)   goto MUL_EXIT;
    result_msg = Test_file_write(Test_file_mul, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_EXIT;

    result_msg = Test_BI_MUL_SUCCESS;

MUL_EXIT:
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}


msg test_bi_mul_karachuba(OUT double* total_time_mul_karachuba, IN bigint** a, IN bigint** b, IN char* str, IN int* karachuba_flag){
    bigint *c = NULL;
    msg result_msg = Test_BI_MUL_KARACHUBA_SUCCESS;
    ParamType param_types[3] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_INT_PTR};

    if (bigint_to_hex(str, a) == -1)   goto MUL_KARACHUBA_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_mul_karachuba, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_mul_karachuba, " * ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT;

    if (bigint_to_hex(str, b) == -1)   goto MUL_KARACHUBA_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_mul_karachuba, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_mul_karachuba, " = ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT;

    *total_time_mul_karachuba += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_mul_karachuba, &c, &result_msg, param_types, a, b, karachuba_flag);
    if (result_msg != BI_MUL_SUCCESS)   goto MUL_KARACHUBA_EXIT;

    if (bigint_to_hex(str, &c) == -1)   goto MUL_KARACHUBA_EXIT;
    result_msg = Test_file_write(Test_file_mul_karachuba, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT;

    result_msg = Test_BI_MUL_KARACHUBA_SUCCESS;

MUL_KARACHUBA_EXIT:
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg compare_multiplicaiton(int start_size, int end_size, int step_size){
    printf("\n=== Comparing Multiplication Methods ===\n");
    printf("Size\titeration\tClassic(s)\tKaratsuba(s)\tRatio\tCrossover\n");
    printf("------------------------------------------------------------------------------\n");

    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    msg result_msg = COMPARE_MULTIPLICATION_SUCCESS;
    int crossover_found = 0;
    int crossover_point = 0;
    int karachuba_flag = 0;
    ParamType param_types[2] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};
    ParamType param_types_karachuba[3] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_INT_PTR};

    for (int word_size = start_size; word_size <= end_size; word_size += step_size){
        double total_time_classic = 0;
        double total_time_karatsuba = 0;

        for (int i = 0; i < test_size; i++){
            result_msg = bi_get_random(&a, word_size);
            if (result_msg != BI_GET_RANDOM_SUCCESS)
                goto COMAPARE_MUL_EXIT;
            else if(a->word_len != word_size){
                result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
                goto COMAPARE_MUL_EXIT;
            }

            result_msg = bi_get_random(&b, word_size);
            if (result_msg != BI_GET_RANDOM_SUCCESS)
                goto COMAPARE_MUL_EXIT;
            else if(b->word_len != word_size){
                result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
                goto COMAPARE_MUL_EXIT;
            }

            total_time_classic += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_mul, &c, &result_msg, param_types, &a, &b);
            if (result_msg != BI_MUL_SUCCESS)
                goto COMAPARE_MUL_EXIT;

            karachuba_flag = word_size / mul_karachuba_ratio;
            total_time_karatsuba += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_mul_karachuba, &c, &result_msg, param_types_karachuba, &a, &b, &karachuba_flag);
            if (result_msg != BI_MUL_SUCCESS)
                goto COMAPARE_MUL_EXIT;
        }
        double avg_time_classic = total_time_classic / test_size;
        double avg_time_karatsuba = total_time_karatsuba / test_size;
        double ratio = avg_time_classic / avg_time_karatsuba;

        // Crossover point 찾기 (Karatsuba가 더 빨라지는 지점)
        if (!crossover_found && ratio > 1.0){
            crossover_found = 1;
            crossover_point = word_size;
        }

        printf("%d\t%d\t\t%.6f\t%.6f\t%.2f\t%s\n",
               word_size,
               test_size,
               avg_time_classic,
               avg_time_karatsuba,
               ratio,
               (word_size == crossover_point) ? "<=== Crossover" : "");
    }

    if (crossover_found){
        printf("\nKaratsuba becomes faster at word size: %d\n\n", crossover_point);
    }
    result_msg = COMPARE_MULTIPLICATION_SUCCESS;

COMAPARE_MUL_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg compare_squaring(int start_size, int end_size, int step_size){
    printf("\n=== Comparing Squaring Methods ===\n");
    printf("Size\titeration\tClassic(s)\tKaratsuba(s)\tRatio\tCrossover\n");
    printf("------------------------------------------------------------------------------\n");

    bigint *a = NULL;
    bigint *c = NULL;
    msg result_msg = COMPARE_SQUARING_SUCCESS;
    int crossover_found = 0;
    int crossover_point = 0;
    int squ_karachuba_flag = 0;
    ParamType param_types[1] = {TYPE_BIGINT_PTR};
    ParamType param_types_karachuba[2] = {TYPE_BIGINT_PTR, TYPE_INT_PTR};

    for (int word_size = start_size; word_size <= end_size; word_size += step_size){
        double total_time_classic = 0;
        double total_time_karatsuba = 0;

        for (int i = 0; i < test_size; i++){
            result_msg = bi_get_random(&a, word_size);
            if (result_msg != BI_GET_RANDOM_SUCCESS)
                goto COMAPARE_SQU_EXIT;
            else if(a->word_len != word_size){
                result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
                goto COMAPARE_SQU_EXIT;
            }

            total_time_classic += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_squ, &c, &result_msg, param_types, &a);
            if (result_msg != BI_SQU_SUCCESS)
                goto COMAPARE_SQU_EXIT;

            squ_karachuba_flag = word_size / squ_karachuba_ratio;
            total_time_karatsuba += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_squ_karachuba, &c, &result_msg, param_types_karachuba, &a, &squ_karachuba_flag);
            if (result_msg != BI_SQU_SUCCESS)
                goto COMAPARE_SQU_EXIT;
        }
        double avg_time_classic = total_time_classic / test_size;
        double avg_time_karatsuba = total_time_karatsuba / test_size;
        double ratio = avg_time_classic / avg_time_karatsuba;

        // Crossover point 찾기 (Karatsuba가 더 빨라지는 지점)
        if (!crossover_found && ratio > 1.0){
            crossover_found = 1;
            crossover_point = word_size;
        }

        printf("%d\t%d\t\t%.6f\t%.6f\t%.2f\t%s\n",
               word_size,
               test_size,
               avg_time_classic,
               avg_time_karatsuba,
               ratio,
               (word_size == crossover_point) ? "<=== Crossover" : "");
    }

    if (crossover_found){
        printf("\nKaratsuba becomes faster at word size: %d\n\n", crossover_point);
    }
    result_msg = COMPARE_SQUARING_SUCCESS;

COMAPARE_SQU_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg compare_division(int start_size, int end_size, int step_size){
    printf("\n=== Comparing Division Methods ===\n");
    printf("Size\titeration\tbinary Long(s)\tWord Long(s)\tRatio\tCrossover\n");
    printf("------------------------------------------------------------------------------\n");

    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    bigint *d = NULL;
    msg result_msg = COMPARE_DIVISION_SUCCESS;
    int crossover_found = 0;
    int crossover_point = 0;
    int div_option = 0;
    ParamType param_types[4] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_INT_PTR};

    for (int word_size = start_size; word_size <= end_size; word_size += step_size){
        double total_time_binary = 0;
        double total_time_word = 0;

        for (int i = 0; i < test_size; i++){
            result_msg = bi_get_random(&a, word_size);
            if (result_msg != BI_GET_RANDOM_SUCCESS)
                goto COMAPARE_DIV_EXIT;
            else if(a->word_len != word_size){
                result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
                goto COMAPARE_DIV_EXIT;
            }

            result_msg = bi_get_random(&b, word_size);
            if (result_msg != BI_GET_RANDOM_SUCCESS)
                goto COMAPARE_DIV_EXIT;
            else if(b->word_len != word_size){
                result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
                goto COMAPARE_DIV_EXIT;
            }
            div_option = 0;
            total_time_binary += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_div, &d, &result_msg, param_types, &c, &a, &b, &div_option);
            if (result_msg != BI_DIV_SUCCESS)
                goto COMAPARE_DIV_EXIT;

            div_option = 1;
            total_time_word += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_div, &d, &result_msg, param_types, &c, &a, &b, &div_option);
            if (result_msg != BI_DIV_SUCCESS)
                goto COMAPARE_DIV_EXIT;
        }
        double avg_time_binary = total_time_binary / test_size;
        double avg_time_word = total_time_word / test_size;
        double ratio = avg_time_binary / avg_time_word;

        // Crossover point 찾기 (Karatsuba가 더 빨라지는 지점)
        if (!crossover_found && ratio > 1.0){
            crossover_found = 1;
            crossover_point = word_size;
        }

        printf("%d\t%d\t\t%.6f\t%.6f\t%.2f\t%s\n",
               word_size,
               test_size,
               avg_time_binary,
               avg_time_word,
               ratio,
               (word_size == crossover_point) ? "<=== Crossover" : "");
    }

    if (crossover_found){
        printf("\nKaratsuba becomes faster at word size: %d\n\n", crossover_point);
    }
    result_msg = COMPARE_DIVISION_SUCCESS;

COMAPARE_DIV_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&d) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    log_msg(result_msg);
    return result_msg;
}


msg test_bi_div(OUT double* total_time_div, IN bigint** a, IN bigint** b, IN char* str, IN int* option){
    bigint *q = NULL;
    bigint *r = NULL;
    msg result_msg = Test_BI_DIV_SUCCESS;
    ParamType param_types[4] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_INT_PTR};

    if (bigint_to_hex(str, a) == -1)   goto DIV_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_div, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_div, " / ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT;

    if (bigint_to_hex(str, b) == -1)   goto DIV_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_div, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_div, " = ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT;

    *total_time_div += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_div, &q, &result_msg, param_types, &r, a, b, option);
    if(result_msg == BI_DIV_BY_ZERO){
        result_msg = Test_file_write(Test_file_div, "DIVISION BY ZERO", APPEND);
        goto DIV_EXIT;
    }

    if (result_msg != BI_DIV_SUCCESS)   goto DIV_EXIT;

    if (bigint_to_hex(str, &q) == -1)   goto DIV_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_div, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_div, ", ", APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT;

    if (bigint_to_hex(str, &r) == -1)   goto DIV_EXIT;
    result_msg = Test_file_write(Test_file_div, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT;

    result_msg = Test_BI_DIV_SUCCESS;

DIV_EXIT:
    if (bi_delete(&q) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&r) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_squ(OUT double* total_time_squ, IN bigint** a, IN char* str){
    bigint *b = NULL;
    msg result_msg = Test_BI_SQU_SUCCESS;
    ParamType param_types[1] = {TYPE_BIGINT_PTR};

    if (bigint_to_hex(str, a) == -1)   goto SQU_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_squ, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)goto SQU_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_squ, " * ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_squ, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_squ, " = ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT;

    *total_time_squ += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_squ, &b, &result_msg, param_types, a);
    if (result_msg != BI_SQU_SUCCESS)   goto SQU_EXIT;

    if (bigint_to_hex(str, &b) == -1)   goto SQU_EXIT;
    result_msg = Test_file_write(Test_file_squ, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT;

    result_msg = Test_BI_SQU_SUCCESS;

SQU_EXIT:
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_squ_karachuba(OUT double* total_time_squ_karachuba, IN bigint** a, IN char* str, IN int* squ_karachuba_flag){
    bigint *b = NULL;
    msg result_msg = Test_BI_SQU_KARACHUBA_SUCCESS;
    ParamType param_types[2] = {TYPE_BIGINT_PTR, TYPE_INT_PTR};

    if (bigint_to_hex(str, a) == -1)   goto SQU_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_squ_karachuba, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)goto SQU_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_squ_karachuba, " * ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_squ_karachuba, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_squ_karachuba, " = ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT;

    *total_time_squ_karachuba += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_squ_karachuba, &b, &result_msg, param_types, a, squ_karachuba_flag);
    if (result_msg != BI_SQU_SUCCESS)   goto SQU_EXIT;

    if (bigint_to_hex(str, &b) == -1)   goto SQU_EXIT;
    result_msg = Test_file_write(Test_file_squ_karachuba, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT;

    result_msg = Test_BI_SQU_KARACHUBA_SUCCESS;

SQU_EXIT:
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_exp(OUT double total_time_exp[3], IN bigint** a, IN bigint** b, IN bigint** c, IN char* str){
    bigint *d = NULL; // 결과 bigint
    msg result_msg = Test_BI_EXP_FAIL;
    ParamType param_types[3] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};

    (*a)->sign = 0; // 여기도 일단 양수로만 하자

    if (bigint_to_hex(str, a) == -1)   goto EXP_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_exp, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)goto EXP_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_exp, " ^ ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT;

    (*b)->sign = 0; // 양수를 기반으로 수행
    (*c)->sign = 0; // 양수를 기반으로 수행

    if (bigint_to_hex(str, b) == -1)   goto EXP_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_exp, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_exp, " mod ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT;

    if(bigint_to_hex(str, c) == -1) goto EXP_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_exp, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_exp, " = ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT;

    // Multipliation Squaring 구현
    total_time_exp[0] += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_exp_MS, &d, &result_msg, param_types, a, b, c);
    if (result_msg != BI_EXP_MS_SUCCESS)   goto EXP_EXIT;

    if (bigint_to_hex(str, &d) == -1)   goto EXP_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_exp, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_exp, " , ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT;

    // Right to Left 구현
    total_time_exp[1] += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_exp_R_TO_L, &d, &result_msg, param_types, a, b, c);
    if (result_msg != BI_EXP_R_TO_L_SUCCESS)   goto EXP_EXIT;

    if (bigint_to_hex(str, &d) == -1)   goto EXP_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_exp, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_exp, " , ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT;

    // Left to Right 구현
    total_time_exp[2] += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_exp_L_TO_R, a, &result_msg, param_types, a, b, c);
    if (result_msg != BI_EXP_L_TO_R_SUCCESS)   goto EXP_EXIT;

    if (bigint_to_hex(str, a) == -1)   goto EXP_EXIT;
    result_msg = Test_file_write(Test_file_exp, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT;

    result_msg = Test_BI_EXP_SUCCESS;

EXP_EXIT:
    if (bi_delete(&d) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_barrett_reduction(OUT double* total_time_barret_reduction, IN bigint** a, IN bigint** barret_N, IN bigint** barret_T, IN char* str){
    bigint *b = NULL;
    msg result_msg = Test_BI_BARRETT_REDUCTION_FAIL;

    ParamType param_types[3] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};

    if (bigint_to_hex(str, a) == -1)   goto BARRET_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_barrett_reduction, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)goto BARRET_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_barrett_reduction, " mod ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto BARRET_EXIT;

    if (bigint_to_hex(str, barret_N) == -1)   goto BARRET_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_barrett_reduction, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)goto BARRET_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_barrett_reduction, " = ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto BARRET_EXIT;

    *total_time_barret_reduction += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())barret_reduction, &b, &result_msg, param_types, a, barret_N, barret_T);
    if (result_msg != BI_BARRET_REDUCTION_SUCCESS)   goto BARRET_EXIT;

    if (bigint_to_hex(str, &b) == -1)   goto BARRET_EXIT;
    result_msg = Test_file_write(Test_file_barrett_reduction, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto BARRET_EXIT;

    result_msg = Test_BI_BARRETT_REDUCTION_SUCCESS;

BARRET_EXIT:
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}


msg test_bi_gcd(OUT double* total_time_div, IN bigint** a, IN bigint** b, IN char* str){
    bigint *d = NULL;
    msg result_msg = Test_BI_GCD_FAIL;
    ParamType param_types[2] = {TYPE_BIGINT_PTR,TYPE_BIGINT_PTR};

    result_msg = Test_file_write_non_enter(Test_file_gcd, "gcd ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto GCD_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_gcd, "( ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto GCD_EXIT;
    
    if (bigint_to_hex(str, a) == -1)   goto GCD_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_gcd, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto GCD_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_gcd, " , ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto GCD_EXIT;

     if (bigint_to_hex(str, b) == -1)   goto GCD_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_gcd, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto GCD_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_gcd, " ) ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto GCD_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_gcd, "= ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto GCD_EXIT;
   
    *total_time_div += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_gcd, &d, &result_msg, param_types, a, b);
    if (result_msg != BI_GCD_SUCCESS)   goto GCD_EXIT;

    if (bigint_to_hex(str, &d) == -1)   goto GCD_EXIT;
    result_msg = Test_file_write(Test_file_gcd, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto GCD_EXIT;

    result_msg = Test_BI_GCD_SUCCESS;

GCD_EXIT:
    if (bi_delete(&d) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_EEA(OUT double* total_time_eea, IN bigint** a, IN bigint** b, IN char* str){
    bigint *gcd = NULL;
    bigint *x = NULL;
    bigint *y = NULL;
    msg result_msg = Test_BI_EEA_FAIL;
    ParamType param_types[4] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};

    result_msg = Test_file_write_non_enter(Test_file_EEA, "gcd ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_EEA, "( ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    if (bigint_to_hex(str, a) == -1)   goto EEA_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_EEA, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_EEA, " , ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    if (bigint_to_hex(str, b) == -1)   goto EEA_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_EEA, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_EEA, " ) ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_EEA, "= ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    *total_time_eea += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_EEA, &gcd, &result_msg, param_types, &x, &y, a, b);
    if (result_msg != BI_EEA_SUCCESS)   goto EEA_EXIT;

    if (bigint_to_hex(str, &gcd) == -1)   goto EEA_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_EEA, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_EEA, " , ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    if (bigint_to_hex(str, &x) == -1)   goto EEA_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_EEA, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_EEA, " , ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    if (bigint_to_hex(str, &y) == -1)   goto EEA_EXIT;
    result_msg = Test_file_write(Test_file_EEA, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EEA_EXIT;

    result_msg = Test_BI_EEA_SUCCESS;

EEA_EXIT:
    if (bi_delete(&gcd) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&x) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&y) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg test_miller_rabin(double* total_time_MR , IN bigint** a, IN int* iteration, char* str){
    msg result_msg = Test_MILLER_RABIN_FAIL;
    ParamType param_types[2] = {TYPE_BIGINT_PTR, TYPE_INT_PTR};
    bigint* temp = NULL;

    *total_time_MR += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())miller_rabin_primality, &temp, &result_msg, param_types, a, iteration);
    if (result_msg != MR_SUCCESS)   goto EXIT_MR;

    if(temp->a[0])  result_msg = Test_file_write_non_enter(Test_file_MR, "Probably_Prime ", APPEND);
    else    result_msg = Test_file_write_non_enter(Test_file_MR, "Composite ", APPEND);

    if (bigint_to_hex(str, a) == -1)   goto EXIT_MR;
    result_msg = Test_file_write(Test_file_MR, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto EXIT_MR;

    result_msg = bi_delete(&temp);
    if (result_msg != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return Test_MILLER_RABIN_SUCCESS;

EXIT_MR:
    if (bi_delete(&temp) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}