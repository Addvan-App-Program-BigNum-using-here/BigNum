#include "bench_util.h"

double time_gcd = 0;
double time_EEA = 0;
double time_MR = 0;
double time_RSA = 0;
double time_RSA_CRT = 0;

int main(){
    bigint* a = NULL;
    bigint* b = NULL;
    bigint* c = NULL;
    bigint* d = NULL;
    bigint* temp = NULL;
    msg result_msg = 0;
    int iteration = 10;
    ParamType param_types_1[1] = {TYPE_BIGINT_PTR};
    ParamType param_types_2[2] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};
    ParamType param_types_4[4] = {TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_BIGINT_PTR, TYPE_BIGINT_PTR};

    // create karachuba pool
    init_karachuba_pool(test_word_size);

    for(int i = 0; i < TEST_LOOP; i++){
        printProgressBar((i + 1) * 100 / TEST_LOOP, 100);

        // create test vector
        get_random_bigint(RANDOM_TEST_WORD_SIZE, test_word_size, test_word_size_limit, 2, &a, &b);

        // gcd bench
        time_gcd += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_gcd, &d, &result_msg, param_types_2, &a, &b);

        // miller rabin bench
        param_types_2[1] = TYPE_INT_PTR;
        time_MR += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())miller_rabin_primality, &temp, &result_msg, param_types_2, &a, &iteration);
        param_types_2[1] = TYPE_BIGINT_PTR;

        // RSA bench
        time_RSA += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())RSA, &d, &result_msg, param_types_1, &a);

        // RSA CRT bench
        time_RSA_CRT += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())RSA_CRT, &d, &result_msg, param_types_1, &a);

        // EEA bench
        a->sign = 0;
        b->sign = 0;
        time_EEA += CHECK_FUNCTION_RUN_ONE_TIME((msg (*)())bi_EEA, &d, &result_msg, param_types_4, &c, &temp, &a, &b);
    }

    // delete karachuba pool
    clear_karachuba_pool();

    // delete bigint
    bi_delete(&a);
    bi_delete(&b);
    bi_delete(&c);
    bi_delete(&d);
    bi_delete(&temp);

    print_data_set();

    printf("\n============ Testing gcd ============\n");
    printf("Time taken gcd : %f seconds\n", time_gcd / TEST_LOOP);

    printf("\n============ Testing EEA ============\n");
    printf("Time taken EEA : %f seconds\n", time_EEA / TEST_LOOP);

    printf("\n============ Testing Miller_Rabin ============\n");
    printf("Time taken Miller Rabin : %f seconds\n", time_MR / TEST_LOOP);

    printf("\n============ Testing RSA ============\n");
    printf("Time taken RSA : %f seconds\n", time_RSA / TEST_LOOP);

    printf("\n============ Testing RSA CRT ============\n");
    printf("Time taken RSA CRT : %f seconds\n", time_RSA_CRT / TEST_LOOP);
}