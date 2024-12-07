#include "func_cmp_test.h"

msg func_compare(){
    if(compare_multiplicaiton(16, 120, 16) != COMPARE_MULTIPLICATION_SUCCESS)   return Test_FAIL;   // bigint 곱셈 성능 비교 테스트
    if(compare_squaring(16, 120, 16) != COMPARE_SQUARING_SUCCESS)   return Test_FAIL;   // bigint 곱셈 성능 비교 테스트
    if(compare_division(16, 120, 16) != COMPARE_DIVISION_SUCCESS)   return Test_FAIL;   // bigint 곱셈 성능 비교 테스트
    return Test_SUCCESS;
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