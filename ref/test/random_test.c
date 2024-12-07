#include "random_test.h"

msg get_random_bigint(int count, ...){
    msg result = BI_GET_RANDOM_FAIL;
    va_list args;
    va_start(args, count);
    byte temp[1] = {0};

    // bigint 할당 및 각 bigint에 대한 word_size 할당
    bigint*** bigints = malloc(count * sizeof(bigint**));
    int* test_word_sizes = malloc(count * sizeof(int));

    // 각 bigint에 할당할 사이즈 결정
    for (int i = 0; i < count; i++) {
        bigints[i] = va_arg(args, bigint**);
        if (test_word_size <= 0) {
            do {
                if (randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS) {
                    result = GEN_RANDOM_BYTES_FAIL;
                    goto cleanup;
                }
                test_word_sizes[i] = temp[0] % test_word_size_limit;
            } while (test_word_sizes[i] <= 0);
        } else  test_word_sizes[i] = test_word_size;
    }

    // bigint 랜덤 생성
    for (int i = 0; i < count; i++) {
        result = bi_get_random(bigints[i], test_word_sizes[i]);
        if (result != BI_GET_RANDOM_SUCCESS)    goto cleanup;
    }

    result = BI_GET_RANDOM_SUCCESS;

cleanup:
    va_end(args);
    free(bigints);
    free(test_word_sizes);
    return result;
}