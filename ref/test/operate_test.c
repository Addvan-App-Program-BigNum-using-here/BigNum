#include "operate_test.h"

double op_total_time[9] = {0, };
double op_exp_time[3] = {0, };

msg cmp_operate_test(){
    msg result_msg = Test_FAIL;
    bigint *big_nums[3] = {NULL, };
    bigint* barret_N = NULL;
    bigint* barret_T = NULL;
    int count = 0;
    int flag = 1;
    char* str = NULL;
    FILE *fp = NULL;

    for(int i = 0; i < 9; i++)  op_total_time[i] = 0;
    for(int i = 0; i < 3; i++)  op_exp_time[i] = 0;

    str = (char *)calloc((test_word_size * 8) * 4 + 100, sizeof(char));
    if(str == NULL)    return MEM_NOT_ALLOC;

    // barret N 생성
    if(test_word_size == barret_word_size){
        result_msg = init_barret_N(&barret_T, &barret_N, barret_word_size);
        if (result_msg != INIT_BARRET_N_SUCCESS){
            log_msg(result_msg);
            return Test_FAIL;
        }
    }

    // karachuba pool 초기화
    if(init_karachuba_pool(test_word_size) != INIT_KARACHUBA_POOL_SUCCESS){
        log_msg(INIT_KARACHUBA_POOL_FAIL);
        return Test_FAIL;
    }

    // 빅넘 테스트 벡터 생성
    // result_msg = new_bigint_test_vector();
    // if(result_msg != new_bigint_test_vector_SUCCESS){
    //     log_msg(new_bigint_test_vector_FAIL);
    //     goto CMP_EXIT;
    // }

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
            result_msg = operate_test(&big_nums[0], &big_nums[1], &big_nums[2], &barret_N, &barret_T);
            if(result_msg != OPERATE_TEST_SUCCESS)  goto CMP_EXIT;

            // 카운터 초기화
            count = 0;
            flag++;
        }
    }

    // karachuba pool 해제
    if(clear_karachuba_pool() != CLEAR_KARACHUBA_POOL_SUCCESS){
        log_msg(CLEAR_KARACHUBA_POOL_FAIL);
        return Test_FAIL;
    }

    operate_print_result();

    result_msg = Test_SUCCESS;
CMP_EXIT:
    fclose(fp);
    free(str);
    bi_delete(&big_nums[0]);
    bi_delete(&big_nums[1]);
    bi_delete(&big_nums[2]);
    return result_msg;
}

msg rand_operate_test(){
    msg result_msg = Test_FAIL;
    bigint* a = NULL;
    bigint* b = NULL;
    bigint* c = NULL;
    bigint* barret_N = NULL;
    bigint* barret_T = NULL;

    for(int i = 0; i < 9; i++)  op_total_time[i] = 0;
    for(int i = 0; i < 3; i++)  op_exp_time[i] = 0;

    // barret N 생성
    if(test_word_size == barret_word_size){
        result_msg = init_barret_N(&barret_T, &barret_N, barret_word_size);
        if (result_msg != INIT_BARRET_N_SUCCESS){
            log_msg(result_msg);
            return Test_FAIL;
        }
    }

    // karachuba pool 초기화
    if(init_karachuba_pool(test_word_size) != INIT_KARACHUBA_POOL_SUCCESS){
        log_msg(INIT_KARACHUBA_POOL_FAIL);
        return Test_FAIL;
    }

    for(int i = 0; i < test_size; i++){
        // 랜덤 bigint 생성
        if(get_random_bigint(3, &a, &b, &c) != BI_GET_RANDOM_SUCCESS)    return Test_FAIL;

        // bigint 연산 테스트
        result_msg = operate_test(&a, &b, &c, &barret_N, &barret_T);
        if(result_msg != OPERATE_TEST_SUCCESS)  goto EXIT;
    }

    // karachuba pool 해제
    if(clear_karachuba_pool() != CLEAR_KARACHUBA_POOL_SUCCESS){
        log_msg(CLEAR_KARACHUBA_POOL_FAIL);
        return Test_FAIL;
    }

    operate_print_result();

    result_msg = Test_SUCCESS;
EXIT:
    bi_delete(&a);
    bi_delete(&b);
    bi_delete(&c);
    return result_msg;
}

msg operate_test(IN bigint** a, IN bigint** b, IN bigint** c, IN bigint** barret_N, IN bigint** barret_T){
    char* str = NULL;
    int karachuba_flag = 0;
    int DIVISION_METHOD = 0;
    msg result_msg = OPERATE_TEST_FAIL;
    int test_max_word_size = (*a)->word_len > (*b)->word_len ? (*a)->word_len : (*b)->word_len;

    str = (char *)calloc((test_word_size * 8) * 4 + 100, sizeof(char));
    if(str == NULL)    return MEM_NOT_ALLOC;

    // // add test
    // result_msg = test_bi_add(&op_total_time[0], a, b, str);
    // if(result_msg != Test_BI_ADD_SUCCESS){
    //    log_msg(Test_BI_ADD_FAIL);
    //    goto OPERATE_EXIT;
    // }

    // // sub test
    // memset(str, 0, (test_word_size * 8) * 4 + 100); // str 초기화
    // result_msg = test_bi_sub(&op_total_time[1], a, b, str);
    // if(result_msg != Test_BI_SUB_SUCCESS){
    //    log_msg(Test_BI_SUB_FAIL);
    //    goto OPERATE_EXIT;
    // }

    // // mul test
    // memset(str, 0, (test_word_size * 8) * 4 + 100); // str 초기화
    // result_msg = test_bi_mul(&op_total_time[2], a, b, str);
    // if(result_msg != Test_BI_MUL_SUCCESS){
    //    log_msg(Test_BI_MUL_FAIL);
    //    goto OPERATE_EXIT;
    // }

    // // mul karachuba test
    // memset(str, 0, (test_word_size * 8) * 4 + 100); // str 초기화
    // karachuba_flag = test_max_word_size / mul_karachuba_ratio;
    // result_msg = test_bi_mul_karachuba(&op_total_time[3], a, b, str, &karachuba_flag);
    // if(result_msg != Test_BI_MUL_KARACHUBA_SUCCESS){
    //    log_msg(Test_BI_MUL_KARACHUBA_FAIL);
    //    goto OPERATE_EXIT;
    // }

    // div test (Binary Long Division)
    DIVISION_METHOD = 0;
    memset(str, 0, (test_word_size * 8) * 4 + 100); // str 초기화
    result_msg = test_bi_div(&op_total_time[4], a, b, str, &DIVISION_METHOD);
    if(result_msg != Test_BI_DIV_SUCCESS){
       log_msg(Test_BI_DIV_FAIL);
       goto OPERATE_EXIT;
    }

    // div test (Word Long Division)
    DIVISION_METHOD = 1;
    memset(str, 0, (test_word_size * 8) * 4 + 100); // str 초기화
    result_msg = test_bi_div(&op_total_time[5], a, b, str, &DIVISION_METHOD);
    if(result_msg != Test_BI_DIV_SUCCESS){
       log_msg(Test_BI_DIV_FAIL);
       goto OPERATE_EXIT;
    }

    // // squ test
    // memset(str, 0, (test_word_size * 8) * 4 + 100); // str 초기화
    // result_msg = test_bi_squ(&op_total_time[6], a, str);
    // if(result_msg != Test_BI_SQU_SUCCESS){
    //    log_msg(Test_BI_SQU_FAIL);
    //    goto OPERATE_EXIT;
    // }

    // // squ karachuba test
    // memset(str, 0, (test_word_size * 8) * 4 + 100); // str 초기화
    // result_msg = test_bi_squ_karachuba(&op_total_time[7], a, str, &karachuba_flag);
    // if(result_msg != Test_BI_SQU_KARACHUBA_SUCCESS){
    //    log_msg(Test_BI_SQU_KARACHUBA_FAIL);
    //    goto OPERATE_EXIT;
    // }

    // // exp test
    // memset(str, 0, (test_word_size * 8) * 4 + 100); // str 초기화
    // result_msg = test_bi_exp(op_exp_time, a, b, c, str);
    // if(result_msg != Test_BI_EXP_SUCCESS){
    //    log_msg(Test_BI_EXP_FAIL);
    //    goto OPERATE_EXIT;
    // }

    // // barret test
    // if(test_word_size == barret_word_size){ // 사전 연산 값이 고정되어 있기에 test_word_size가 기존 사이즈와 같을 때만 수행
    //     memset(str, 0, (test_word_size * 8) * 4 + 100); // str 초기화
    //     result_msg = test_bi_barrett_reduction(&op_total_time[8], a, barret_N, barret_T, str);
    //     if(result_msg != Test_BI_BARRETT_REDUCTION_SUCCESS){
    //         log_msg(Test_BI_BARRETT_REDUCTION_FAIL);
    //         goto OPERATE_EXIT;
    //     }
    // }

    result_msg = OPERATE_TEST_SUCCESS;
OPERATE_EXIT:
    free(str);
    return result_msg;
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

void operate_print_result(){
    print_data_set();

    // printf("\n============ Testing bi_add ============\n");
    // printf("Time taken add : %f seconds\n", op_total_time[0] / test_size);

    // printf("\n============ Testing bi_sub ============\n");
    // printf("Time taken sub : %f seconds\n", op_total_time[1] / test_size);

    // printf("\n============ Testing bi_mul ============\n");
    // printf("Time taken mul : %f seconds\n", op_total_time[2] / test_size);

    // printf("\n============ Testing bi_mul_karachuba ============\n");
    // printf("Time taken mul_karachuba : %f seconds\n", op_total_time[3] / test_size);

    printf("\n============ Testing bi_div (Binary Long Division) ============\n");
    printf("Time taken div(Binary) : %f seconds\n", op_total_time[4] / test_size);

    printf("\n============ Testing bi_div (WORD Long Division) ============\n");
    printf("Time taken div(Long) : %f seconds\n", op_total_time[5] / test_size);

    // printf("\n============ Testing bi_squ ============\n");
    // printf("Time taken squ : %f seconds\n", op_total_time[6] / test_size);

    // printf("\n============ Testing bi_squ_karachuba ============\n");
    // printf("Time taken squ_karachuba : %f seconds\n", op_total_time[7] / test_size);

    // printf("\n============ Testing bi_exp ============\n");
    // printf("Time taken exp (MS) : %f seconds\n", op_exp_time[0] / test_size);
    // printf("Time taken exp (R TO L) : %f seconds\n", op_exp_time[1] / test_size);
    // printf("Time taken exp (L TO R) : %f seconds\n", op_exp_time[2] / test_size);

    // printf("\n============ Testing bi_barrett_reduction ============\n");
    // printf("Time taken barret_reduction : %f seconds\n\n", op_total_time[8] / test_size);
}