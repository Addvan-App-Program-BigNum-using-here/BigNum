#include "util_test.h"

struct timeval start, end;
double time_used;

msg util_test(){
    if(test_bi_new_delete() != Test_BI_NEW_DELETE_SUCCESS)    return Test_FAIL;
    if(test_bi_random() != Test_BI_GET_RANDOM_SUCCESS)    return Test_FAIL;
    if(test_bi_set_from() != Test_BI_SET_FROM_SUCCESS)    return Test_FAIL;
    if(test_bi_shift() != Test_BI_SHIFT_SUCCESS)    return Test_FAIL;
    if(test_bi_get_lower() != Test_BI_GET_LOWER_SUCCESS)    return Test_FAIL;
    if(test_bi_cat() != Test_BI_CAT_SUCCESS)    return Test_FAIL;
    return Test_SUCCESS;
}

msg new_bigint_test_vector(){
    msg result_msg = new_bigint_test_vector_FAIL;
    char* str = NULL;
    bigint* a = NULL;

    // file clear
    if(Test_file_write_non_enter(Test_bigint, "", CLEAR) != FILE_WRITE_SUCCESS)   return new_bigint_test_vector_FAIL;

    str = (char *)calloc((test_word_size * 8) * 4 + 100, sizeof(char));
    if (str == NULL)    return MEM_NOT_ALLOC;

    for(int i = 0; i < test_size; i++){
        for(int j = 0; j < 3; j++){
            result_msg = get_random_bigint(1, &a);
            if(result_msg != BI_GET_RANDOM_SUCCESS)    goto NEW_BIGINT_EXIT;
            bigint_to_hex(str, &a);
            Test_file_write_non_enter(Test_bigint, str, APPEND);
            Test_file_write_non_enter(Test_bigint, "\n", APPEND);
        }
    }
    result_msg = new_bigint_test_vector_SUCCESS;
NEW_BIGINT_EXIT:
    if (str != NULL)    free(str);
    if (a != NULL)    bi_delete(&a);
    return result_msg;
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