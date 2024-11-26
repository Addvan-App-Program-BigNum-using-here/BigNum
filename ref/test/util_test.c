#include "util_test.h"

struct timeval start, end;
double time_used;

int main(){
    FILE *fp = NULL;

    // 시작 log
    if (Test_file_write(TEST_init, CLEAR) != FILE_WRITE_SUCCESS){
        log_msg(FILE_WRITE_FAIL);
        return Test_FAIL;
    }

    test_bi_new_delete();       // bigint 할당 및 해제 테스트
    test_bi_random();           // 랜덤 bigint 생성 테스트
    test_bi_set_from();         // string으로부터 bigint 생성 테스트
    test_bi_shift();            // bigint shift 테스트
    test_bi_get_lower();        // bigint modular 테스트
    test_bi_cat();              // bigint cat 테스트
    test_bi_add();              // bigint 덧셈 테스트
    test_bi_sub();              // bigint 뺄셈 테스트
    test_bi_div();              // bigint 나눗셈 테스트
    test_bi_squ();              // bigint 제곱 테스트
    test_bi_exp();              // bigint exponetion 테스트
    test_bi_mul();              // bigint 곱셈 테스트

    // ************** bigint 곱셈 karachuba 테스트 **************
    // bigint 곱셈 karachub 테스트
    if(init_karachuba_pool(test_word_size) != INIT_KARACHUBA_POOL_SUCCESS){
        log_msg(INIT_KARACHUBA_POOL_FAIL);
        return Test_FAIL;
    }
    test_bi_mul_karachuba();

    if(clear_karachuba_pool() != CLEAR_KARACHUBA_POOL_SUCCESS){
        log_msg(CLEAR_KARACHUBA_POOL_FAIL);
        return Test_FAIL;
    }
    // **********************************************************

    if(compare_multiplicaiton(10, 100, 10) != COMPARE_MULTIPLICATION_SUCCESS)   return Test_FAIL;   // bigint 곱셈 성능 비교 테스트

    // 종료 Log
    if (Test_file_write(TEST_end, APPEND) != FILE_WRITE_SUCCESS){
        log_msg(FILE_WRITE_FAIL);
        return Test_FAIL;
    }

    log_msg(Test_SUCCESS);

    // Sage test
    fp = popen("sage ../../sage_test/test.sage >/dev/null 2>&1", "r");
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
}

msg test_bi_new_delete(){
    bigint *a = NULL;
    int rand_test_word_size = test_word_size;
    msg result_msg = 0;

    printf("\n============ Testing bi_new_delete ============\n");
    gettimeofday(&start, NULL);

    for (int i = 0; i < test_size; i++){
        // test_word_size가 0보다 작거나 같으면 랜덤으로 rand_test_word_size를 할당해준다. (0 ~ 99)
        if(test_word_size <= 0){
            // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
            do{
                byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
                result_msg = randombytes(temp, 1);
                if(result_msg != GEN_RANDOM_BYTES_SUCCESS)    goto EXIT_NEW_DEL;
                rand_test_word_size = temp[0] % test_word_size_limit;
            }while(rand_test_word_size <= 0);
        }

        if(bi_new(&a, rand_test_word_size) != BI_ALLOC_SUCCESS)
            return BI_ALLOC_FAIL;
        if(a->word_len != rand_test_word_size){
            result_msg = bi_delete(&a);
            if(result_msg != BI_FREE_SUCCESS)    goto EXIT_NEW_DEL;
            return BI_ALLOC_FAIL;
        }
        result_msg = bi_delete(&a);
        if(result_msg != BI_FREE_SUCCESS)   goto EXIT_NEW_DEL;
    }
    result_msg = Test_BI_NEW_DELETE_SUCCESS;

EXIT_NEW_DEL:
    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_NEW_DELETE_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_shift(){
    int rand_test_word_size = test_word_size;
    msg result_msg = Test_BI_SHIFT_SUCCESS;
    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    bigint *d = NULL;
    int shift_size_left = 0;
    int shift_size_right = 0;
    byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
    char* shift_left_size_str = NULL;
    char* shift_right_size_str = NULL;
    char* str = NULL;

    printf("\n============ Testing bi_shfit ============\n");
    gettimeofday(&start, NULL);

    if (Test_file_write(SHIFT_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for(int i = 0; i < test_size; i++){
        if(test_word_size <= 0){
            // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                rand_test_word_size = temp[0] % test_word_size_limit;
            }while(rand_test_word_size <= 0);
        }

       // left shift size
        if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
        shift_size_left = temp[0] % (rand_test_word_size * WORD_BITS);
        shift_left_size_str = int_to_string(shift_size_left);

        // right shift size
        if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
        shift_size_right = temp[0] % (rand_test_word_size * WORD_BITS);
        shift_right_size_str = int_to_string(shift_size_right);

        str = (char*)calloc(2 * rand_test_word_size * WORD_BITS + 1, sizeof(char));
        if (str == NULL)    return MEM_NOT_ALLOC;

        // 랜덤한 bigint 생성
        result_msg = bi_get_random(&a, rand_test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto SHIFT_EXIT_FREE;
        else if(a->word_len != rand_test_word_size){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto SHIFT_EXIT_FREE;
        }

        // 랜덤한 bigint 생성
        result_msg = bi_get_random(&b, rand_test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto SHIFT_EXIT_FREE;
        else if(b->word_len != rand_test_word_size){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto SHIFT_EXIT_FREE;
        }

        if (bigint_to_hex(str, &a) == -1)   goto SHIFT_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // shift left test
        result_msg = Test_file_write_non_enter(" << ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // shift size 저장
        result_msg = Test_file_write_non_enter(shift_left_size_str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // left shift 수행
        result_msg = bi_shift_left(&c, &a, shift_size_left);
        if (result_msg != BI_SHIFT_SUCCESS)   goto SHIFT_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // 결과 값 저장
        if (bigint_to_hex(str, &c) == -1)   goto SHIFT_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;


        // shift right test
        if (bigint_to_hex(str, &b) == -1)   goto SHIFT_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // shift left test
        result_msg = Test_file_write_non_enter(" >> ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // shift size 저장
        result_msg = Test_file_write_non_enter(shift_right_size_str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // right shift 수행
        result_msg = bi_shift_right(&d, &b, shift_size_right);
        if (result_msg != BI_SHIFT_SUCCESS)   goto SHIFT_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        // 결과 값 저장
        if (bigint_to_hex(str, &d) == -1)   goto SHIFT_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SHIFT_EXIT_FREE;

        free(str);
        free(shift_right_size_str);
        free(shift_left_size_str);
    }
    result_msg = Test_BI_SHIFT_SUCCESS;
    goto SHIFT_EXIT;

SHIFT_EXIT_FREE:
    free(str);
    free(shift_left_size_str);
    free(shift_right_size_str);
SHIFT_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&d) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;

    if (bi_delete(&a) != BI_FREE_SUCCESS)   result_msg = BI_FREE_FAIL;
    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_SHIFT_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
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

    printf("\n============ Testing bi_get_lower ============\n");
    gettimeofday(&start, NULL);

    if(test_word_size > 0){
        if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
        lower_bit_size = temp[0] % (rand_test_word_size * WORD_BITS);
        lower_bit_str = int_to_string(lower_bit_size);
    }

    if (Test_file_write(GET_LOWER_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for(int i = 0; i < test_size; i++){
        if(test_word_size <= 0){
            // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
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
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto GET_LOWER_EXIT_FREE;

        // get_lower test
        result_msg = Test_file_write_non_enter(" & ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto GET_LOWER_EXIT_FREE;

        // lower size 저장
        result_msg = Test_file_write_non_enter(lower_bit_str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto GET_LOWER_EXIT_FREE;

        // get_lower 수행
        result_msg = bi_get_lower(&b, &a, lower_bit_size);
        if (result_msg != BI_GET_LOWER_SUCCESS)   goto GET_LOWER_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto GET_LOWER_EXIT_FREE;

        // 결과 값 저장
        if (bigint_to_hex(str, &b) == -1)   goto GET_LOWER_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
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

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_GET_LOWER_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
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

    printf("\n============ Testing cat ============\n");
    gettimeofday(&start, NULL);

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

    if (Test_file_write(CAT_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

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
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto CAT_EXIT_FREE;
        // get_lower test
        result_msg = Test_file_write_non_enter(" || ", APPEND);
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
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto CAT_EXIT_FREE;
        // get_lower 수행
        result_msg = bi_cat(&c, &a, &b);
        if (result_msg != BI_CAT_SUCCESS)   goto CAT_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto CAT_EXIT_FREE;

        // 결과 값 저장
        if (bigint_to_hex(str, &c) == -1)   goto CAT_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
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

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_CAT_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_set_from(){
    bigint *a = NULL;
    word *test_array = NULL;
    int result_msg = Test_BI_SET_FROM_SUCCESS;
    int rand_test_word_size = test_word_size;

    printf("\n============ Testing bi_set_from ============\n");
    gettimeofday(&start, NULL);

    if (Test_file_write(FROM_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

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

        // little endian set test
        result_msg = bi_set_from_array(&a, 1, rand_test_word_size, test_array, little_endian);
        if (result_msg != BI_SET_ARRAY_SUCCESS) goto FROM_EXIT_TEST_ARRAY;

        // 할당이 잘 되었는지 확인
        for (int i = 0; i < rand_test_word_size; i++){
            if (a->a[i] != test_array[i]){
                result_msg = BI_SET_ARRAY_NOT_MATCH;
                goto FROM_EXIT_TEST_ARRAY;
            }
        }

        // big_endian set test
        result_msg = bi_set_from_array(&a, 1, rand_test_word_size, test_array, big_endian);
        if (result_msg != BI_SET_ARRAY_SUCCESS) goto FROM_EXIT_TEST_ARRAY;

        for (int i = 0; i < rand_test_word_size; i++){
            if (a->a[(rand_test_word_size - 1) - i] != test_array[i]){
                result_msg = BI_SET_ARRAY_NOT_MATCH;
                goto FROM_EXIT_TEST_ARRAY;
            }
        }
    }

    // 2진수 테스트
    result_msg = Test_file_write("[2]", APPEND);
    if(result_msg != FILE_WRITE_SUCCESS)    goto FROM_EXIT;
    result_msg = test_bi_set_from_base(2);
    if(result_msg != Test_BI_SET_FROM_BASE_SUCCESS)    goto FROM_EXIT;

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)   goto FROM_EXIT;

    // 10진수 테스트
    result_msg = Test_file_write("[10]", APPEND);
//    if(result_msg != FILE_WRITE_SUCCESS)    goto FROM_EXIT;
//    result_msg = test_bi_set_from_base(10);
//    if(result_msg != Test_BI_SET_FROM_BASE_SUCCESS)    goto FROM_EXIT;

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)   goto FROM_EXIT;

    // 16진수 테스트
    result_msg = Test_file_write("[16]", APPEND);
    if(result_msg != FILE_WRITE_SUCCESS)    goto FROM_EXIT;;
    result_msg = test_bi_set_from_base(16);
    if(result_msg != Test_BI_SET_FROM_BASE_SUCCESS)    goto FROM_EXIT;;

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)   goto FROM_EXIT;

    result_msg = Test_BI_SET_FROM_SUCCESS;
    goto FROM_EXIT;

FROM_EXIT_TEST_ARRAY:
    free(test_array);
FROM_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   result_msg = BI_FREE_FAIL;
    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_SET_FROM_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}


msg test_bi_set_from_base(const IN int base){
    bigint *a = NULL;
    char *str = NULL;
    char *str_base = NULL;
    msg result_msg = 0;
    int array_size = test_word_size; // 최대 4바이트 크기의 정수

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
        result_msg = get_random_string(&str_base, array_size, base); // 오버플로우 나면 여기 먼저 수정
        if(result_msg != RAND_STRING_SUCCESS) goto FROM_BASE_EXIT_FREE;

        // 랜덤한 문자열 저장
        result_msg = Test_file_write(str_base, APPEND); // 랜덤한 문자열 저장
        if (result_msg != FILE_WRITE_SUCCESS)   goto FROM_BASE_EXIT_FREE;

        // base 테스트
        result_msg = bi_set_from_string(&a, str_base, base); // base 테스트
        if (result_msg != BI_SET_STRING_SUCCESS)    goto FROM_BASE_EXIT_FREE;

        // bigint to hex string
        if (bigint_to_hex(str, &a) == -1)   goto FROM_BASE_EXIT_FREE;

        // 변환한 16진수 문자열을 txt에 넣는다.
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto FROM_BASE_EXIT_FREE;

        free(str);
    }
    result_msg = Test_BI_SET_FROM_BASE_SUCCESS;
    goto FROM_BASE_EXIT;

FROM_BASE_EXIT_FREE:
    free(str);
FROM_BASE_EXIT:
    free(str_base);
    if (bi_delete(&a) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_random(){
    bigint *dst = NULL;
    msg result_msg = 0;
    int rand_test_word_size = test_word_size;

    printf("\n============ Testing bi_random ============\n");
    gettimeofday(&start, NULL);

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

        result_msg = bi_get_random(&dst, rand_test_word_size);
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
    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_GET_RANDOM_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_add(){
    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_ADD_SUCCESS;

    int test_word_size_a = test_word_size;
    int test_word_size_b = test_word_size;
    int test_max_word_size = max(test_word_size_a, test_word_size_b);

    printf("\n============ Testing bi_add ============\n");
    gettimeofday(&start, NULL);

    if (Test_file_write(add_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for (int i = 0; i < test_size; i++){
        // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
        if(test_word_size <= 0){
            byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_a = temp[0] % test_word_size_limit;
            }while(test_word_size_a <= 0);
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_b = temp[0] % test_word_size_limit;
            }while(test_word_size_b <= 0);
            test_max_word_size = max(test_word_size_a, test_word_size_b);
        }

        str = (char *)calloc((test_max_word_size * 8) * 3 + 100, sizeof(char)); // '0x' * 3 + 부호 * 3 + " + " + " = " => 6 + 3 + 3 + 3 = 15
        if (str == NULL)    return MEM_NOT_ALLOC;

        result_msg = bi_get_random(&a, test_word_size_a);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto ADD_EXIT_FREE;
        else if(a->word_len != test_word_size_a){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto ADD_EXIT_FREE;
        }

        result_msg = bi_get_random(&b, test_word_size_b);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto ADD_EXIT_FREE;
        else if(b->word_len != test_word_size_b){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto ADD_EXIT_FREE;
        }

        if (bigint_to_hex(str, &a) == -1)   goto ADD_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto ADD_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" + ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto ADD_EXIT_FREE;

        if (bigint_to_hex(str, &b) == -1)   goto ADD_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto ADD_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto ADD_EXIT_FREE;

        result_msg = bi_add(&c, &a, &b);
        if (result_msg != BI_ADD_SUCCESS)   goto ADD_EXIT_FREE;

        if (bigint_to_hex(str, &c) == -1)   goto ADD_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto ADD_EXIT_FREE;

        free(str);
    }
    result_msg = Test_BI_ADD_SUCCESS;
    goto ADD_EXIT;

ADD_EXIT_FREE:
    free(str);
ADD_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_ADD_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_sub(){
    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_SUB_SUCCESS;

    int test_word_size_a = test_word_size;
    int test_word_size_b = test_word_size;
    int test_max_word_size = max(test_word_size_a, test_word_size_b);

    printf("\n============ Testing bi_sub ============\n");
    gettimeofday(&start, NULL);

    if (Test_file_write(sub_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for (int i = 0; i < test_size; i++){
        // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
        if(test_word_size <= 0){
            byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_a = temp[0] % test_word_size_limit;
            }while(test_word_size_a <= 0);
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_b = temp[0] % test_word_size_limit;
            }while(test_word_size_b <= 0);
            test_max_word_size = max(test_word_size_a, test_word_size_b);
        }

        str = (char *)calloc((test_max_word_size * 8) * 3 + 100, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
        if (str == NULL)    return MEM_NOT_ALLOC;

        result_msg = bi_get_random(&a, test_word_size_a);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto SUB_EXIT_FREE;
        else if(a->word_len != test_word_size_a){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto SUB_EXIT_FREE;
        }

        result_msg = bi_get_random(&b, test_word_size_b);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto SUB_EXIT_FREE;
        else if(b->word_len != test_word_size_b){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto SUB_EXIT_FREE;
        }

        if (bigint_to_hex(str, &a) == -1)   goto SUB_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SUB_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" - ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SUB_EXIT_FREE;

        if (bigint_to_hex(str, &b) == -1)   goto SUB_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SUB_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SUB_EXIT_FREE;

        result_msg = bi_sub(&c, &a, &b);
        if (result_msg != BI_SUB_SUCCESS)   goto SUB_EXIT_FREE;

        if (bigint_to_hex(str, &c) == -1)   goto SUB_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SUB_EXIT_FREE;

        free(str);
    }
    result_msg = Test_BI_SUB_SUCCESS;
    goto SUB_EXIT;

SUB_EXIT_FREE:
    free(str);
SUB_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_SUB_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_mul(){
    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_MUL_SUCCESS;

    int test_word_size_a = test_word_size;
    int test_word_size_b = test_word_size;
    int test_max_word_size = max(test_word_size_a, test_word_size_b);

    printf("\n============ Testing bi_mul ============\n");
    gettimeofday(&start, NULL);

    if (Test_file_write(mul_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for (int i = 0; i < test_size; i++){
        // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
        if(test_word_size <= 0){
            byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_a = temp[0] % test_word_size_limit;
            }while(test_word_size_a <= 0);
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_b = temp[0] % test_word_size_limit;
            }while(test_word_size_b <= 0);
            test_max_word_size = max(test_word_size_a, test_word_size_b);
        }

        str = (char *)calloc((test_max_word_size * 8) * 3 + 100, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
        if (str == NULL)    return MEM_NOT_ALLOC;

        result_msg = bi_get_random(&a, test_word_size_a);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto MUL_EXIT_FREE;
        else if(a->word_len != test_word_size_a){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto MUL_EXIT_FREE;
        }

        result_msg = bi_get_random(&b, test_word_size_b);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto MUL_EXIT_FREE;
        else if(b->word_len != test_word_size_b){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto MUL_EXIT_FREE;
        }

        if (bigint_to_hex(str, &a) == -1)   goto MUL_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" * ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_EXIT_FREE;

        if (bigint_to_hex(str, &b) == -1)   goto MUL_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_EXIT_FREE;

        result_msg = bi_mul(&c, &a, &b);
        if (result_msg != BI_MUL_SUCCESS)   goto MUL_EXIT_FREE;

        if (bigint_to_hex(str, &c) == -1)   goto MUL_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_EXIT_FREE;

        free(str);
    }
    result_msg = Test_BI_MUL_SUCCESS;
    goto MUL_EXIT;

MUL_EXIT_FREE:
    free(str);
MUL_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_MUL_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_mul_karachuba(){
    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_MUL_KARACHUBA_SUCCESS;
    int test_word_size_a = test_word_size;
    int test_word_size_b = test_word_size;
    int test_max_word_size = max(test_word_size_a, test_word_size_b);

    printf("\n============ Testing bi_mul_karachuba ============\n");
    gettimeofday(&start, NULL);

    if (Test_file_write(mul_karachuba_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for (int i = 0; i < test_size; i++){
        // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
        if(test_word_size <= 0){
            byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_a = temp[0] % test_word_size_limit;
            }while(test_word_size_a <= 0);
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_b = temp[0] % test_word_size_limit;
            }while(test_word_size_b <= 0);
            test_max_word_size = max(test_word_size_a, test_word_size_b);
        }

        str = (char *)calloc((test_max_word_size * 8) * 3 + 100, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
        if (str == NULL)    return MEM_NOT_ALLOC;

        result_msg = bi_get_random(&a, test_word_size_a);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto MUL_KARACHUBA_EXIT_FREE;
        else if(a->word_len != test_word_size_a){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto MUL_KARACHUBA_EXIT_FREE;
        }

        result_msg = bi_get_random(&b, test_word_size_b);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto MUL_KARACHUBA_EXIT_FREE;
        else if(b->word_len != test_word_size_b){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto MUL_KARACHUBA_EXIT_FREE;
        }

        if (bigint_to_hex(str, &a) == -1)   goto MUL_KARACHUBA_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" * ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        if (bigint_to_hex(str, &b) == -1)   goto MUL_KARACHUBA_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        result_msg = bi_mul_karachuba(&c, &a, &b);
        if (result_msg != BI_MUL_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        if (bigint_to_hex(str, &c) == -1)   goto MUL_KARACHUBA_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        free(str);
    }
    result_msg = Test_BI_MUL_KARACHUBA_SUCCESS;
    goto MUL_KARACHUBA_EXIT;

MUL_KARACHUBA_EXIT_FREE:
    free(str);
MUL_KARACHUBA_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_MUL_KARACHUBA_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_squ_karachuba(){
    bigint *a = NULL;
    bigint *c = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_MUL_KARACHUBA_SUCCESS;
    int test_word_size_a = test_word_size;
    printf("\n============ Testing bi_squ_karachuba ============\n");
    gettimeofday(&start, NULL);

    if (Test_file_write(mul_karachuba_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for (int i = 0; i < test_size; i++){
        // test_word_size가 0보다 작거나 같으면 랜덤으로 test_word_size를 할당해준다. (0 ~ 99)
        if(test_word_size <= 0){
            byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_a = temp[0] % test_word_size_limit;
            }while(test_word_size_a <= 0);

        str = (char *)calloc((test_word_size_a * 8) * 3 + 100, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
        if (str == NULL)    return MEM_NOT_ALLOC;

        result_msg = bi_get_random(&a, test_word_size_a);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto MUL_KARACHUBA_EXIT_FREE;
        else if(a->word_len != test_word_size_a){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto MUL_KARACHUBA_EXIT_FREE;
        }

        if (bigint_to_hex(str, &a) == -1)   goto MUL_KARACHUBA_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" * ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        if (bigint_to_hex(str, &a) == -1)   goto MUL_KARACHUBA_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        result_msg = bi_squ_karachuba(&c, &a);
        if (result_msg != BI_MUL_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        if (bigint_to_hex(str, &c) == -1)   goto MUL_KARACHUBA_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto MUL_KARACHUBA_EXIT_FREE;

        free(str);
    }
    result_msg = Test_BI_MUL_KARACHUBA_SUCCESS;
    goto MUL_KARACHUBA_EXIT;

MUL_KARACHUBA_EXIT_FREE:
    free(str);
MUL_KARACHUBA_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_MUL_KARACHUBA_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}
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

    for (int word_size = start_size; word_size <= end_size; word_size += step_size){
        double total_time_classic = 0;
        double total_time_karatsuba = 0;

        for (int i = 0; i < test_size; i++){
            clock_t start, end;

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

            start = clock();
            result_msg = bi_mul(&c, &a, &b);
            if (result_msg != BI_MUL_SUCCESS)
                goto COMAPARE_MUL_EXIT;
            end = clock();
            total_time_classic += ((double)(end - start)) / CLOCKS_PER_SEC;

            start = clock();
            result_msg = bi_mul_karachuba(&c, &a, &b);
            if (result_msg != BI_MUL_SUCCESS)
                goto COMAPARE_MUL_EXIT;
            end = clock();
            total_time_karatsuba += ((double)(end - start)) / CLOCKS_PER_SEC;
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

msg test_bi_div(){
    bigint *a = NULL;
    bigint *b = NULL;
    bigint *q = NULL;
    bigint *r = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_SQU_SUCCESS;

    int test_word_size_a = test_word_size;
    int test_word_size_b = test_word_size;
    int test_max_word_size = max(test_word_size_a, test_word_size_b);

    printf("\n\n============ Testing bi_div ============\n");
    gettimeofday(&start, NULL);

    if (Test_file_write(div_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for (int i = 0; i < test_size; i++){
        if(test_word_size <= 0){
            byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_a = temp[0] % test_word_size_limit;
            }while(test_word_size_a <= 0);
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_word_size_b = temp[0] % test_word_size_limit;
            }while(test_word_size_b <= 0);
            test_max_word_size = max(test_word_size_a, test_word_size_b);
        }

        str = (char *)calloc((test_max_word_size * 8) * 4 + 100, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
        if (str == NULL)
            return MEM_NOT_ALLOC;

        result_msg = bi_get_random(&a, test_word_size_a);
        if(result_msg != BI_GET_RANDOM_SUCCESS) goto DIV_EXIT_FREE;
        else if(a->word_len != test_word_size_a){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto DIV_EXIT_FREE;
        }

        result_msg = bi_get_random(&b, test_word_size_b);
        if (result_msg != BI_GET_RANDOM_SUCCESS)    goto DIV_EXIT_FREE;
        else if(b->word_len != test_word_size_b){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto DIV_EXIT_FREE;
        }

        if (bigint_to_hex(str, &a) == -1)   goto DIV_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" / ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT_FREE;

        if (bigint_to_hex(str, &b) == -1)   goto DIV_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT_FREE;

        result_msg = bi_div(&q, &r, &a, &b);
        if(result_msg == BI_DIV_BY_ZERO){
            result_msg = Test_file_write("DIVISION BY ZERO", APPEND);
            if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT_FREE;
            continue;
        }
        if (result_msg != BI_DIV_SUCCESS)   goto DIV_EXIT_FREE;

        if (bigint_to_hex(str, &q) == -1)   goto DIV_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT_FREE;

        result_msg = Test_file_write_non_enter(", ", APPEND); // 구분자
        if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT_FREE;

        if (bigint_to_hex(str, &r) == -1)   goto DIV_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto DIV_EXIT_FREE;

        free(str);
    }
    result_msg = Test_BI_DIV_SUCCESS;
    goto DIV_EXIT;

DIV_EXIT_FREE:
    free(str);
DIV_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&q) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&r) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_DIV_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_squ(){
    bigint *a = NULL;
    bigint *b = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_SQU_SUCCESS;
    int test_max_word_size = test_word_size;

    printf("\n============ Testing bi_squ ============\n");
    gettimeofday(&start, NULL);

    if (Test_file_write(SEQ_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for (int i = 0; i < test_size; i++){
       if(test_word_size <= 0){
            byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_max_word_size = temp[0] % test_word_size_limit;
            }while(test_max_word_size <= 0);
        }

       str = (char *)calloc((test_max_word_size * 8) * 2 + 100, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
       if (str == NULL) return MEM_NOT_ALLOC;

        result_msg = bi_get_random(&a, test_max_word_size);
        if(result_msg != BI_GET_RANDOM_SUCCESS) goto SQU_EXIT_FREE;
        else if(a->word_len != test_max_word_size){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto SQU_EXIT_FREE;
        }

        if (bigint_to_hex(str, &a) == -1)   goto SQU_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)goto SQU_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" * ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT_FREE;

        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT_FREE;

        result_msg = bi_squ(&b, &a);
        if (result_msg != BI_SQU_SUCCESS)   goto SQU_EXIT_FREE;

        if (bigint_to_hex(str, &b) == -1)   goto SQU_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto SQU_EXIT_FREE;

        free(str);
    }
    result_msg = Test_BI_SQU_SUCCESS;
    goto SQU_EXIT;

SQU_EXIT_FREE:
    free(str);

SQU_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_SQU_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

msg test_bi_exp(){
    bigint *a = NULL; // exp 하는 bigint
    bigint *b = NULL; // exp 지수 bigint
    bigint *c = NULL; // mod bigint
    bigint *d = NULL; // 결과 bigint

    char *str = NULL;
    msg result_msg = Test_BI_EXP_SUCCESS;
    int test_max_word_size = test_word_size;

    printf("\n============ Testing bi_exp ============\n");
    gettimeofday(&start, NULL);

    if (Test_file_write(EXP_init, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;

    for (int i = 0; i < test_size; i++){
       if(test_word_size <= 0){
            byte temp[1] = {0}; // 랜덤 값을 받아오기 위한
            do{
                if(randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS)    return GEN_RANDOM_BYTES_FAIL;
                test_max_word_size = temp[0] % test_word_size_limit;
            }while(test_max_word_size <= 0);
        }

       str = (char *)calloc((test_max_word_size * 8) * 4 + 100, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
       if (str == NULL) return MEM_NOT_ALLOC;

        // 피연산자를 랜덤으로 생성
        result_msg = bi_get_random(&a, test_max_word_size);
        if(result_msg != BI_GET_RANDOM_SUCCESS) goto EXP_EXIT_FREE;
        else if(a->word_len != test_max_word_size){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto EXP_EXIT_FREE;
        }

        a->sign = 0; // 여기도 일단 양수로만 하자

        if (bigint_to_hex(str, &a) == -1)   goto EXP_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)goto EXP_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" ^ ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT_FREE;

        // 지수를 랜덤으로 생성
        result_msg = bi_get_random(&b, test_max_word_size);
        if(result_msg != BI_GET_RANDOM_SUCCESS) goto EXP_EXIT_FREE;
        else if(b->word_len != test_max_word_size){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto EXP_EXIT_FREE;
        }

        // b 값을 0x10001로 제한
        b->sign = 0; // 일단 지금은 지수가 양수만! 지수가 음수인 경우 역원 찾는거라서 이거는 EEA 구현해야 할 수 있을 듯
        b->a[0] = 0x10001; // 지수가 너무 커지면 너무 오래 걸리니까 255로 제한
        for(int i = b->word_len - 1; i >= 1; i--)   b->a[i] = 0;
        if(bi_refine(&b) != BI_SET_REFINE_SUCCESS) goto EXP_EXIT_FREE;

        if (bigint_to_hex(str, &b) == -1)   goto EXP_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" mod ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT_FREE;

        // mod를 랜덤으로 생성
        result_msg = bi_get_random(&c, test_max_word_size);
        if(result_msg != BI_GET_RANDOM_SUCCESS) goto EXP_EXIT_FREE;
        else if(c->word_len != test_max_word_size){
            result_msg = BI_GET_RANDOM_LENGTH_NOT_MATCH;
            goto EXP_EXIT_FREE;
        }

        c->sign = 0; // 일단 mod도 양수만!
        c->a[0] = 10001; // mod도 너무 커지면 너무 오래 걸리니까 255로 제한

        if(bigint_to_hex(str, &c) == -1) goto EXP_EXIT_FREE;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT_FREE;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT_FREE;

        result_msg = bi_exp_MS(&d, &a, &b, &c);
        if (result_msg != BI_EXP_MS_SUCCESS)   goto EXP_EXIT_FREE;

        if (bigint_to_hex(str, &d) == -1)   goto EXP_EXIT_FREE;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)   goto EXP_EXIT_FREE;

        free(str);
    }
    result_msg = Test_BI_EXP_SUCCESS;
    goto EXP_EXIT;

EXP_EXIT_FREE:
    free(str);

EXP_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;
    if (bi_delete(&d) != BI_FREE_SUCCESS)   return BI_FREE_FAIL;

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_EXP_SUCCESS)   return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);
    if (Test_file_write(seperator, APPEND) != FILE_WRITE_SUCCESS)   return FILE_WRITE_FAIL;
    return Test_SUCCESS;
}

int gettimeofday(struct timeval *tv, struct timezone *tz) {
    struct _timeb timebuffer;
    _ftime_s(&timebuffer);
    
    if (tv) {
        tv->tv_sec = (long)timebuffer.time;
        tv->tv_usec = (long)(timebuffer.millitm * 1000);
    }
    
    // Windows에서는 tinemezo이 사용되지 않으므로, NULL로 설정할 수 있습니다.
    // if (tz) {
    //     tz->tz_minuteswest = 0;
    //     tz->tz_dsttime = 0;
    // }
    return 0;
}