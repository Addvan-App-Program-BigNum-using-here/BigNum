#include "crypto_test.h"


double crypto_total_time[5] = {0, };

msg cmp_crypto_test(){
    msg result_msg = Test_FAIL;
    bigint *big_nums[3] = {NULL, };
    int count = 0;
    int flag = 1;
    FILE *fp = NULL;
    char* str = NULL;

    for(int i = 0; i < 5; i++)  crypto_total_time[i] = 0;
    str = (char *)calloc((test_word_size * 8) * 4 + 100, sizeof(char));
    if(str == NULL)    return MEM_NOT_ALLOC;

    // 빅넘 테스트 벡터 생성
    result_msg = new_bigint_test_vector();
    if(result_msg != new_bigint_test_vector_SUCCESS){
        log_msg(new_bigint_test_vector_FAIL);
        goto CMP_EXIT;
    }

    fp = fopen(Test_bigint, "r");

    while (fgets(str, test_word_size * WORD_BITS, fp) != NULL) {
        str[strcspn(str, "\n")] = '\0';
        int is_negative = (str[0] == '-');
        char *hex_str = is_negative ? str + 3 : str + 2;

        bi_set_from_string(&big_nums[count], hex_str, 16);
        if(is_negative) big_nums[count]->sign = 1;
        count++;
        if (count == 3) {
            printf("%dth test\n", flag);
            // bigint 연산 테스트
            result_msg = crypto_test(&big_nums[0], &big_nums[1]);
            if(result_msg != CRYPTO_TEST_SUCCESS)  goto CMP_EXIT;

            // 카운터 초기화
            count = 0;
            flag++;
        }
    }
    crypto_print_result();

    result_msg = Test_SUCCESS;
CMP_EXIT:
    fclose(fp);
    bi_delete(&big_nums[0]);
    bi_delete(&big_nums[1]);
    bi_delete(&big_nums[2]);
    return result_msg;
}

msg rand_crypto_test(){
    msg result_msg = Test_FAIL;
    bigint* a = NULL;
    bigint* b = NULL;
    bigint* c = NULL;

    for(int i = 0; i < 5; i++)  crypto_total_time[i] = 0;

    // karachuba pool 초기화
    if(init_karachuba_pool(test_word_size) != INIT_KARACHUBA_POOL_SUCCESS){
        log_msg(INIT_KARACHUBA_POOL_FAIL);
        return Test_FAIL;
    }

    for(int i = 0; i < test_size; i++){
        // 랜덤 bigint 생성
        if(get_random_bigint(3, &a, &b, &c) != BI_GET_RANDOM_SUCCESS)    return Test_FAIL;

        // bigint 연산 테스트
        result_msg = crypto_test(&a, &b);
        if(result_msg != CRYPTO_TEST_SUCCESS)  goto EXIT;
    }

    // karachuba pool 해제
    if(clear_karachuba_pool() != CLEAR_KARACHUBA_POOL_SUCCESS){
        log_msg(CLEAR_KARACHUBA_POOL_FAIL);
        return Test_FAIL;
    }
    crypto_print_result();

    result_msg = Test_SUCCESS;
EXIT:
    bi_delete(&a);
    bi_delete(&b);
    bi_delete(&c);
    return result_msg;
}

msg crypto_test(IN bigint** a, IN bigint** b){
    char* str = NULL;
    msg result_msg = CRYPTO_TEST_FAIL;
    int miller_rabin_iter = 10;

    str = (char *)calloc((test_word_size * 8) * 4 + 100, sizeof(char));
    if(str == NULL)    return MEM_NOT_ALLOC;

    // bigint gcd 테스트
    result_msg = test_bi_gcd(&crypto_total_time[0], a, b, str);
    if(result_msg != Test_BI_GCD_SUCCESS){
        log_msg(Test_BI_GCD_FAIL);
        goto CRYPTO_EXIT;
    }

    // bigint EEA 테스트
    (*a)->sign = 0;
    (*b)->sign = 0;
    memset(str, 0, (test_word_size * 8) * 4 + 100);
    result_msg = test_bi_EEA(&crypto_total_time[1], a, b, str);
    if(result_msg != Test_BI_EEA_SUCCESS){
        log_msg(Test_BI_EEA_FAIL);
        goto CRYPTO_EXIT;
    }

    // bigint Miller Rabin 테스트
    (*a)->sign = 0;
    memset(str, 0, (test_word_size * 8) * 4 + 100); // str 초기화
    result_msg = test_miller_rabin(&crypto_total_time[2], a, &miller_rabin_iter, str);
    if(result_msg != Test_MILLER_RABIN_SUCCESS){
        log_msg(Test_MILLER_RABIN_FAIL);
        goto CRYPTO_EXIT;
    }

    // RSA 테스트
    (*a)->sign = 0;
    bi_resize(a, test_word_size - 1); // M은 N보다 1 작은 수
    result_msg = test_RSA(&crypto_total_time[3], a, str);
    if(result_msg != TEST_RSA_SUCCESS){
        log_msg(TEST_RSA_FAIL);
        goto CRYPTO_EXIT;
    }

    // RSA CRT 테스트
    (*a)->sign = 0;
    bi_resize(a, test_word_size - 1); // M은 N보다 1 작은 수
    result_msg = test_RSA_CRT(&crypto_total_time[4], a, str);
    if(result_msg != TEST_RSA_CRT_SUCCESS){
        log_msg(TEST_RSA_CRT_FAIL);
        goto CRYPTO_EXIT;
    }

    result_msg = CRYPTO_TEST_SUCCESS;
CRYPTO_EXIT:
    free(str);
    return result_msg;
}

msg test_RSA(OUT double* total_time_RSA, IN bigint** a, IN char* str){
    msg result_msg = TEST_RSA_FAIL;
    bigint *m2 = NULL;
    ParamType param_types[1] = {TYPE_BIGINT_PTR}; // m

    *total_time_RSA += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())RSA, &m2, &result_msg, param_types, a);
    if(result_msg == RSA_MISSMATCH || result_msg == RSA_FAIL){
        result_msg = Test_file_write_non_enter(Test_file_RSA, "RSA_FAIL ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto RSA_EXIT;
    }else if(result_msg == RSA_SUCCESS){
        result_msg = Test_file_write_non_enter(Test_file_RSA, "RSA_SUCCESS ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto RSA_EXIT;
    }

    if (bigint_to_hex(str, a) == -1)   goto RSA_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_RSA, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto RSA_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_RSA, ",  ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto RSA_EXIT;

    if (bigint_to_hex(str, &m2) == -1)   goto RSA_EXIT;
    result_msg = Test_file_write(Test_file_RSA, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto RSA_EXIT;

    result_msg = TEST_RSA_SUCCESS;
RSA_EXIT:
    if (bi_delete(&m2) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    return result_msg;
}

msg test_RSA_CRT(OUT double* total_time_RSA_CRT, IN bigint** a, IN char* str){
    msg result_msg = TEST_RSA_CRT_FAIL;
    bigint *m2 = NULL;
    ParamType param_types[1] = {TYPE_BIGINT_PTR}; // m

    *total_time_RSA_CRT += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())RSA_CRT, &m2, &result_msg, param_types, a);
    if(result_msg == RSA_MISSMATCH || result_msg == RSA_FAIL){
        result_msg = Test_file_write_non_enter(Test_file_RSA_CRT, "RSA_FAIL ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto RSA_CRT_EXIT;
    }else if(result_msg == RSA_SUCCESS){
        result_msg = Test_file_write_non_enter(Test_file_RSA_CRT, "RSA_SUCCESS ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto RSA_CRT_EXIT;
    }

    if (bigint_to_hex(str, a) == -1)   goto RSA_CRT_EXIT;
    result_msg = Test_file_write_non_enter(Test_file_RSA_CRT, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto RSA_CRT_EXIT;

    result_msg = Test_file_write_non_enter(Test_file_RSA_CRT, ",  ", APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto RSA_CRT_EXIT;

    if (bigint_to_hex(str, &m2) == -1)   goto RSA_CRT_EXIT;
    result_msg = Test_file_write(Test_file_RSA_CRT, str, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)   goto RSA_CRT_EXIT;

    result_msg = TEST_RSA_CRT_SUCCESS;
RSA_CRT_EXIT:
    if (bi_delete(&m2) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
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


void crypto_print_result(){
    print_data_set();

    printf("\n============ Testing bi_gcd ============\n");
    printf("Time taken gcd : %f seconds\n", crypto_total_time[0] / test_size);

    printf("\n============ Testing bi_EEA ============\n");
    printf("Time taken EEA : %f seconds\n", crypto_total_time[1] / test_size);

    printf("\n============ Testing Miller Rabin ============\n");
    printf("Time taken Miller Rabin : %f seconds\n", crypto_total_time[2] / test_size);

    printf("\n============ Testing RSA ============\n");
    printf("Time taken Miller Rabin : %f seconds\n", crypto_total_time[3] / test_size);

    printf("\n============ Testing RSA CRT ============\n");
    printf("Time taken Miller Rabin : %f seconds\n\n", crypto_total_time[4] / test_size);
}