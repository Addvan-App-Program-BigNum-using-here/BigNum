#include "func_cmp_test.h"

msg func_compare(){
    if(compare_multiplicaiton(16, 120, 16) != COMPARE_MULTIPLICATION_SUCCESS)   return Test_FAIL;   // bigint 곱셈 성능 비교 테스트
    if(compare_squaring(16, 120, 16) != COMPARE_SQUARING_SUCCESS)   return Test_FAIL;   // bigint 곱셈 성능 비교 테스트
    if(compare_division(16, 120, 16) != COMPARE_DIVISION_SUCCESS)   return Test_FAIL;   // bigint 곱셈 성능 비교 테스트
    if(compare_exponention(16, 120, 16) != COMPARE_EXPONENTION_SUCCESS)   return Test_FAIL;   // bigint 곱셈 성능 비교 테스트
    return Test_SUCCESS;
}


msg compare_multiplicaiton(int start_size, int end_size, int step_size){
    print_data_set();
    printf("\n=== Comparing Multiplication Methods ===\n");
    printf("Size\titeration\tClassic(s)\tKaratsuba(s)\tRatio\tCrossover\n");
    printf("------------------------------------------------------------------------------\n");

    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    msg result_msg = COMPARE_MULTIPLICATION_FAIL;
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
    print_data_set();
    printf("\n=== Comparing Squaring Methods ===\n");
    printf("Size\titeration\tClassic(s)\tKaratsuba(s)\tRatio\tCrossover\n");
    printf("------------------------------------------------------------------------------\n");

    bigint *a = NULL;
    bigint *c = NULL;
    msg result_msg = COMPARE_SQUARING_FAIL;
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
    print_data_set();
    printf("\n=== Comparing Division Methods ===\n");
    printf("Size\titeration\tbinary Long(s)\tWord Long(s)\tRatio\tCrossover\n");
    printf("------------------------------------------------------------------------------\n");

    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    bigint *d = NULL;
    msg result_msg = COMPARE_DIVISION_FAIL;
    int crossover_found = 0;
    int crossover_point = 0;
    int cmp_div_option = 0;
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
            cmp_div_option = 0;
            total_time_binary += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_div, &d, &result_msg, param_types, &c, &a, &b, &cmp_div_option);
            if (result_msg != BI_DIV_SUCCESS)
                goto COMAPARE_DIV_EXIT;

            cmp_div_option = 1;
            total_time_word += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_div, &d, &result_msg, param_types, &c, &a, &b, &cmp_div_option);
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

msg compare_exponention(int start_size, int end_size, int step_size){
    print_data_set();
    printf("\n=== Comparing Modular exponention Methods ===\n");
    printf("Size\titeration\tMS(s)    \tR TO L(s)\tL TO R(s)\n");
    printf("------------------------------------------------------------------------------\n");

    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    bigint *d = NULL;
    msg result_msg = COMPARE_EXPONENTION_FAIL;
    ParamType param_types[3] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};

    for (int word_size = start_size; word_size <= end_size; word_size += step_size){
        double total_time_MS = 0;
        double total_time_RTL = 0;
        double total_time_LTR = 0;

        for (int i = 0; i < test_size; i++){
            result_msg = bi_get_random(&a, word_size);
            if (result_msg != BI_GET_RANDOM_SUCCESS)
                goto COMAPARE_EXP_EXIT;
            else if(a->word_len != word_size){
                result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
                goto COMAPARE_EXP_EXIT;
            }

            result_msg = bi_get_random(&b, word_size);
            if (result_msg != BI_GET_RANDOM_SUCCESS)
                goto COMAPARE_EXP_EXIT;
            else if(b->word_len != word_size){
                result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
                goto COMAPARE_EXP_EXIT;
            }

            result_msg = bi_get_random(&c, word_size);
            if (result_msg != BI_GET_RANDOM_SUCCESS)
                goto COMAPARE_EXP_EXIT;
            else if(b->word_len != word_size){
                result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
                goto COMAPARE_EXP_EXIT;
            }
            b->sign = 0; // 지수 값은 항상 양수

            total_time_MS += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_exp_MS, &d, &result_msg, param_types, &a, &b, &c);
            if (result_msg != BI_EXP_MS_SUCCESS)
                goto COMAPARE_EXP_EXIT;

            total_time_RTL += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_exp_R_TO_L, &d, &result_msg, param_types, &a, &b, &c);
            if (result_msg != BI_EXP_R_TO_L_SUCCESS)
                goto COMAPARE_EXP_EXIT;

            total_time_LTR += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_exp_L_TO_R, &d, &result_msg, param_types, &a, &b, &c);
            if (result_msg != BI_EXP_L_TO_R_SUCCESS)
                goto COMAPARE_EXP_EXIT;
        }
        double avg_time_MS = total_time_MS / test_size;
        double avg_time_RTL = total_time_RTL / test_size;
        double avg_time_LTR = total_time_LTR / test_size;

        printf("%d\t%d\t\t%.6f\t%.6f\t%.6f\n",
               word_size,
               test_size,
               avg_time_MS,
               avg_time_RTL,
               avg_time_LTR);
    }
    result_msg = COMPARE_EXPONENTION_SUCCESS;

COMAPARE_EXP_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&d) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    log_msg(result_msg);
    return result_msg;
}