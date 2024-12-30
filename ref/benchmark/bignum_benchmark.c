#include "bench_util.h"

double time_add = 0;
double time_sub = 0;
double time_mul = 0;
double time_mul_karachuba = 0;
double time_div_bin = 0;
double time_div_word = 0;
double time_squ = 0;
double time_squ_karachuba = 0;
double time_exp_MS = 0;
double time_exp_RTL = 0;
double time_exp_LTR = 0;
double time_barret_reduction = 0;


int main(){
    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    bigint *d = NULL;
    bigint *r = NULL;
    bigint *barret_N = NULL;
    bigint *barret_T = NULL;
    msg result_msg = 0;
    ParamType param_types_1[1] = {TYPE_BIGINT_PTR};
    ParamType param_types_2[2] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};
    ParamType param_types_3[3] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};
    ParamType param_types_4[4] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_INT_PTR};
    int karachuba_flag = 0;
    int div_mode = 0;

    // barret N 생성
    if(test_word_size == barret_word_size)  init_barret_N(&barret_T, &barret_N, barret_word_size);

    // karachuba pool 초기화
    init_karachuba_pool(test_word_size);

    for(int i = 0; i < TEST_LOOP; i++){
        printProgressBar((i + 1) * 100 / TEST_LOOP, 100);
        // create test vector
        get_random_bigint(RANDOM_TEST_WORD_SIZE, test_word_size, test_word_size_limit, 3, &a, &b, &c);

        // addition bench
        time_add += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_add, &d, &result_msg, param_types_2, &a, &b);

        // subtraction bench
        time_sub += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_sub, &d, &result_msg, param_types_2, &a, &b);

        // multiplication bench
        time_mul += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_mul, &d, &result_msg, param_types_2, &a, &b);

        // karachuba multiplication bench
        param_types_3[2] = TYPE_INT_PTR;
        karachuba_flag = test_word_size / mul_karachuba_ratio;
        time_mul_karachuba += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_mul_karachuba, &d, &result_msg, param_types_3, &a, &b, &karachuba_flag);
        param_types_3[2] = TYPE_BIGINT_PTR;

        // Binary division bench
        time_div_bin += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_div, &d, &result_msg, param_types_4, &r, &a, &b, &div_mode);

        // Word division bench
        div_mode = 1;
        time_div_word += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_div, &d, &result_msg, param_types_4, &r, &a, &b, &div_mode);

        // squaring bench
        time_squ += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_squ, &d, &result_msg, param_types_1, &a);

        // karachuba squaring bench
        param_types_2[1] = TYPE_INT_PTR;
        karachuba_flag = test_word_size / squ_karachuba_ratio;
        time_squ_karachuba += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_squ_karachuba, &d, &result_msg, param_types_2, &a, &karachuba_flag);
        param_types_2[1] = TYPE_BIGINT_PTR;

        // Modular exponentiation bench
        time_exp_MS += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_exp_MS, &d, &result_msg, param_types_3, &a, &b, &c);
        time_exp_LTR += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_exp_R_TO_L, &d, &result_msg, param_types_3, &a, &b, &c);
        time_exp_RTL += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_exp_L_TO_R, &d, &result_msg, param_types_3, &a, &b, &c);

        // barret reduction bench
        time_barret_reduction += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())barret_reduction, &d, &result_msg, param_types_3, &a, &barret_N, &barret_T);
    }

    // karachuba pool 해제
    clear_karachuba_pool();

    // free bigint
    bi_delete(&a);
    bi_delete(&b);
    bi_delete(&c);
    bi_delete(&d);
    bi_delete(&r);
    bi_delete(&barret_N);
    bi_delete(&barret_T);


    print_data_set();

    printf("\n============ Testing bi_add ============\n");
    printf("Time taken add : %f seconds\n", time_add / TEST_LOOP);

    printf("\n============ Testing bi_sub ============\n");
    printf("Time taken sub : %f seconds\n", time_sub / TEST_LOOP);

    printf("\n============ Testing bi_mul ============\n");
    printf("Time taken mul : %f seconds\n", time_mul / TEST_LOOP);

    printf("\n============ Testing bi_mul_karachuba ============\n");
    printf("Time taken mul_karachuba : %f seconds\n", time_mul_karachuba / TEST_LOOP);

    printf("\n============ Testing bi_div (Binary Long Division) ============\n");
    printf("Time taken div(Binary) : %f seconds\n", time_div_bin / TEST_LOOP);

    printf("\n============ Testing bi_div (WORD Long Division) ============\n");
    printf("Time taken div(Long) : %f seconds\n", time_div_word / TEST_LOOP);

    printf("\n============ Testing bi_squ ============\n");
    printf("Time taken squ : %f seconds\n", time_squ / TEST_LOOP);

    printf("\n============ Testing bi_squ_karachuba ============\n");
    printf("Time taken squ_karachuba : %f seconds\n", time_squ_karachuba / TEST_LOOP);

    printf("\n============ Testing bi_exp ============\n");
    printf("Time taken exp (MS) : %f seconds\n", time_exp_MS / TEST_LOOP);
    printf("Time taken exp (R TO L) : %f seconds\n", time_exp_RTL / TEST_LOOP);
    printf("Time taken exp (L TO R) : %f seconds\n", time_exp_LTR / TEST_LOOP);

    printf("\n============ Testing bi_barrett_reduction ============\n");
    printf("Time taken barret_reduction : %f seconds\n\n", time_barret_reduction / TEST_LOOP);
}