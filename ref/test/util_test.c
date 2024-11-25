#include "util_test.h"

int main()
{
    msg result_msg = 0;
    FILE *fp = NULL;
    int test_size = 1;
    int test_word_size = 2;
    struct timeval start, end;
    double time_used;

    result_msg = Test_file_write(TEST_init, CLEAR); // 시작 log
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        log_msg(result_msg);
        return Test_FAIL;
    }

    // bigint 할당 및 해제 테스트
    printf("\n============ Testing bi_new_delete ============\n");
    gettimeofday(&start, NULL);
    result_msg = test_bi_new_delete(test_size, test_word_size);
    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_NEW_DELETE_SUCCESS)
        return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        log_msg(result_msg);
        return Test_FAIL;
    }

    // 랜덤 bigint 생성 테스트
    printf("\n============ Testing bi_random ============\n");
    gettimeofday(&start, NULL);
    result_msg = test_bi_random(test_size, test_word_size);
    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_GET_RANDOM_SUCCESS)
        return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        log_msg(result_msg);
        return Test_FAIL;
    }

    

    // string으로부터 bigint 생성 테스트
    printf("\n============ Testing bi_set_from ============\n");
    gettimeofday(&start, NULL);
    result_msg = test_bi_set_from(test_size, test_word_size);
    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_SET_FROM_SUCCESS)
        return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        log_msg(result_msg);
        return Test_FAIL;
    }

    
    // bigint 덧셈 테스트
    printf("\n============ Testing bi_add ============\n");
    gettimeofday(&start, NULL);
    result_msg = test_bi_add(test_size, test_word_size);
    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_ADD_SUCCESS)
        return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        log_msg(result_msg);
        return Test_FAIL;
    }

    // bigint 뺄셈 테스트
    printf("\n============ Testing bi_sub ============\n");
    gettimeofday(&start, NULL);
    result_msg = test_bi_sub(test_size, test_word_size);
    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_SUB_SUCCESS)
        return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        log_msg(result_msg);
        return Test_FAIL;
    }

    
    // bigint 곱셈 테스트
    printf("\n============ Testing bi_mul ============\n");
    gettimeofday(&start, NULL);
    result_msg = test_bi_mul(test_size, test_word_size);
    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_MUL_SUCCESS)
        return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        log_msg(result_msg);
        return Test_FAIL;
    }
    // printf("\n============ Testing bi_div ============\n");
    // gettimeofday(&start, NULL);
    // result_msg = test_bi_div(test_size, test_word_size);
    // gettimeofday(&end, NULL);
    // time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    // log_msg(result_msg);
    // if (result_msg != Test_BI_DIV_SUCCESS)
    //     return Test_FAIL;
    // printf("Time taken: %f seconds\n", time_used);

    // bigint 곱셈 karachub 테스트
    printf("\n============ Testing bi_mul_karachuba ============\n");
    gettimeofday(&start, NULL);

    // karachuba 실행
    result_msg = init_karachuba_pool(test_word_size);
    if(result_msg != INIT_KARACHUBA_POOL_SUCCESS)    return Test_FAIL;
    result_msg = test_bi_mul_karachuba(test_size, test_word_size);
    if(result_msg != Test_BI_MUL_KARACHUBA_SUCCESS)   return Test_FAIL;
    result_msg = clear_karachuba_pool();
    if(result_msg != CLEAR_KARACHUBA_POOL_SUCCESS)    return Test_FAIL;

    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    printf("Time taken: %f seconds\n", time_used);

    result_msg = Test_file_write(seperator, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        log_msg(result_msg);
        return Test_FAIL;
    }

    // bigint 곱셈 성능 비교 테스트
    result_msg = compare_multiplicaiton(10, test_word_size, 10, test_size);
    if(result_msg != COMPARE_MULTIPLICATION_SUCCESS)   return Test_FAIL;

    printf("\n");
    result_msg = Test_file_write(TEST_end, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        log_msg(result_msg);
        return Test_FAIL;
    }
    
     // bigint 제곱 테스트
    printf("\n============ Testing bi_squ ============\n");
    gettimeofday(&start, NULL);
    result_msg = test_bi_squ(test_size, test_word_size);
    gettimeofday(&end, NULL);
    time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    log_msg(result_msg);
    if (result_msg != Test_BI_SQU_SUCCESS)
        return Test_FAIL;
    printf("Time taken: %f seconds\n", time_used);

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        log_msg(result_msg);
        return Test_FAIL;
    }

    log_msg(Test_SUCCESS);

    // Sage test
    fp = popen("sage ../../sage_test/test.sage >/dev/null 2>&1", "r");
    if (fp == NULL)
    {
        printf("Failed to run command\n");
        return 1;
    }

    // Close the command stream
    int status = pclose(fp);
    if (status == -1)
    {
        perror("pclose failed");
        return 1;
    }
    else
    {
        printf("python script exited with status %d\n", status);
    }

    return 0;
}

msg test_bi_new_delete(const IN int test_size, const IN int test_word_size)
{
    bigint *a = NULL;
    for (int i = 0; i < test_size; i++)
    {
        if (bi_new(&a, test_word_size) == BI_ALLOC_FAIL || a->word_len != test_word_size)
            return BI_ALLOC_FAIL;
        if (bi_delete(&a) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
    }
    return Test_BI_NEW_DELETE_SUCCESS;
}

msg test_bi_set_from(const IN int test_size, const IN int test_word_size)
{
    bigint *a = NULL;
    word *test_array = NULL;
    int result_msg = Test_BI_SET_FROM_SUCCESS;

    result_msg = Test_file_write(FROM_init, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)
        return result_msg;

    // 테스트 할 배열 생성
    test_array = (word *)calloc(test_word_size, sizeof(word));
    if (test_array == NULL)
        return MEM_NOT_ALLOC;

    // 문자열로 들어온 hex를 endian에 맞추어 잘 저장하는지 테스트
    for (int i = 0; i < test_size; i++)
    {
        // 랜덤한 배열 생성
        result_msg = array_random(test_array, test_word_size);
        if (result_msg != GEN_RANDOM_SUCCESS)
            goto FROM_EXIT;

        // little endian set test
        result_msg = bi_set_from_array(&a, 1, test_word_size, test_array, little_endian);
        if (result_msg != BI_SET_ARRAY_SUCCESS)
            goto FROM_EXIT;

        // 할당이 잘 되었는지 확인
        for (int i = 0; i < test_word_size; i++)
        {
            if (a->a[i] != test_array[i])
            {
                result_msg = BI_SET_ARRAY_NOT_MATCH;
                goto FROM_EXIT;
            }
        }

        // big_endian set test
        result_msg = bi_set_from_array(&a, 1, test_word_size, test_array, big_endian);
        if (result_msg != BI_SET_ARRAY_SUCCESS)
            goto FROM_EXIT;

        for (int i = 0; i < test_word_size; i++)
        {
            if (a->a[(test_word_size - 1) - i] != test_array[i])
            {
                result_msg = BI_SET_ARRAY_NOT_MATCH;
                goto FROM_EXIT;
            }
        }
    }

    // 2진수 테스트
    result_msg = Test_file_write("[2]", APPEND);
    if(result_msg != FILE_WRITE_SUCCESS)    goto FROM_EXIT;
    result_msg = test_bi_set_from_base(test_size, 2, test_word_size);
    if(result_msg != Test_BI_SET_FROM_BASE_SUCCESS)    goto FROM_EXIT;

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)
        goto FROM_EXIT;

    // 10진수 테스트
    result_msg = Test_file_write("[10]", APPEND);
//    if(result_msg != FILE_WRITE_SUCCESS)    goto FROM_EXIT;
//    result_msg = test_bi_set_from_base(test_size, 10, test_word_size);
//    if(result_msg != Test_BI_SET_FROM_BASE_SUCCESS)    goto FROM_EXIT;

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)
        goto FROM_EXIT;
    ;

    // 16진수 테스트
    result_msg = Test_file_write("[16]", APPEND);
    if(result_msg != FILE_WRITE_SUCCESS)    goto FROM_EXIT;;
    result_msg = test_bi_set_from_base(test_size, 16, test_word_size);
    if(result_msg != Test_BI_SET_FROM_BASE_SUCCESS)    goto FROM_EXIT;;

    result_msg = Test_file_write(seperator, APPEND); // 구분자
    if (result_msg != FILE_WRITE_SUCCESS)
        goto FROM_EXIT;
    ;

    result_msg = Test_BI_SET_FROM_SUCCESS;
    goto FROM_EXIT;

FROM_EXIT:
    free(test_array);
    if (bi_delete(&a) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    return result_msg;
}


msg test_bi_set_from_base(const IN int test_size, const IN int base, const IN int test_word_size){
    byte byte_array_size[4]; // 4바이트 크기의 배열
    word array_size = 0;     // 최대 4바이트 크기의 정수
    bigint *a = NULL;
    char *str = NULL;
    char *str_base = NULL;
    msg result_msg = 0;

    // 랜덤한 array_size 할당
//    if(randombytes(byte_array_size, 4) != GEN_RANDOM_BYTES_SUCCESS)  return GEN_RANDOM_BYTES_FAIL;
//    array_size = byte_to_uint(byte_array_size, 4); // 길이가 랜덤으로 잡힌다.
//    if(!array_size)    return SET_ARRAY_SIZE_FAIL;
//    array_size %= 1000;

    array_size = test_word_size;

    str = (char*)calloc(array_size * 32 + 20, sizeof(byte)); // '0x' * 3 + '-' * 3 + " + " + " = " => 6 + 3 + 3 + 3 = 14
    if(str == NULL) return MEM_NOT_ALLOC;

    for (int i = 0; i < test_size; i++)
    {
        // base 기준 랜덤한 str 가져오기
        result_msg = get_random_string(&str_base, array_size, base); // 오버플로우 나면 여기 먼저 수정
        if(result_msg != RAND_STRING_SUCCESS) goto FROM_BASE_EXIT;

        // 랜덤한 문자열 저장
        result_msg = Test_file_write(str_base, APPEND); // 랜덤한 문자열 저장
        if (result_msg != FILE_WRITE_SUCCESS)
            goto FROM_BASE_EXIT;

        // base 테스트
        result_msg = bi_set_from_string(&a, str_base, base); // base 테스트
        if (result_msg != BI_SET_STRING_SUCCESS)
            goto FROM_BASE_EXIT;

        // bigint to hex string
        if (bigint_to_hex(&a, str) == -1)
            goto FROM_BASE_EXIT;

        // 변환한 16진수 문자열을 txt에 넣는다.
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            goto FROM_BASE_EXIT;

        result_msg = bi_delete(&a);
        if (result_msg != BI_FREE_SUCCESS)
            goto FROM_BASE_EXIT;
    }
    result_msg = Test_BI_SET_FROM_BASE_SUCCESS;
    goto FROM_BASE_EXIT;

FROM_BASE_EXIT:
    free(str);
    free(str_base);
    if (bi_delete(&a) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_random(const IN int test_size, const IN int test_word_size)
{
    bigint *dst = NULL;
    msg result_msg = 0;

    for (int i = 0; i < test_size; i++)
    {
        result_msg = bi_get_random(&dst, test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS)
        {
            if (bi_delete(&dst) != BI_FREE_SUCCESS)
                return BI_FREE_FAIL;
            log_msg(result_msg);
            return Test_BI_GET_RANDOM_FAIL;
        }
        if (bi_delete(&dst) != BI_FREE_SUCCESS)
            return BI_FREE_FAIL;
    }
    return Test_BI_GET_RANDOM_SUCCESS;
}

msg test_bi_add(const IN int test_size, const IN int test_word_size)
{
    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_ADD_SUCCESS;

    str = (char *)calloc((test_word_size * 8) * 3 + 100, sizeof(char)); // '0x' * 3 + 부호 * 3 + " + " + " = " => 6 + 3 + 3 + 3 = 15
    if (str == NULL)
        return MEM_NOT_ALLOC;

    result_msg = Test_file_write(add_init, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        free(str);
        return result_msg;
    }

    for (int i = 0; i < test_size; i++)
    {
        result_msg = bi_get_random(&a, test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS || a->word_len != test_word_size)
            break;

        result_msg = bi_get_random(&b, test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS || b->word_len != test_word_size)
            break;

        if (bigint_to_hex(&a, str) == -1)
            break;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = Test_file_write_non_enter(" + ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        if (bigint_to_hex(&b, str) == -1)
            break;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = bi_add(&c, &a, &b);
        if (result_msg != BI_ADD_SUCCESS)
            break;

        if (bigint_to_hex(&c, str) == -1)
            break;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = bi_delete(&a);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = bi_delete(&b);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = bi_delete(&c);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = Test_BI_ADD_SUCCESS;
    }

    free(str);
    if (bi_delete(&a) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_sub(const IN int test_size, const IN int test_word_size)
{
    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_SUB_SUCCESS;

    str = (char *)calloc((test_word_size * 8) * 3 + 100, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
    if (str == NULL)
        return MEM_NOT_ALLOC;

    result_msg = Test_file_write(sub_init, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        free(str);
        return result_msg;
    }

    for (int i = 0; i < test_size; i++)
    {
        result_msg = bi_get_random(&a, test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS || a->word_len != test_word_size)
            break;

        result_msg = bi_get_random(&b, test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS || b->word_len != test_word_size)
            break;

        if (bigint_to_hex(&a, str) == -1)
            break;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = Test_file_write_non_enter(" - ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        if (bigint_to_hex(&b, str) == -1)
            break;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = bi_sub(&c, &a, &b);
        if (result_msg != BI_SUB_SUCCESS)
            break;

        if (bigint_to_hex(&c, str) == -1)
            break;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = bi_delete(&a);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = bi_delete(&b);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = bi_delete(&c);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = Test_BI_SUB_SUCCESS;
    }

    free(str);
    if (bi_delete(&a) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_mul(const IN int test_size, const IN int test_word_size)
{
    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_MUL_SUCCESS;

    str = (char *)calloc((test_word_size * 8) * 3 + 100, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
    if (str == NULL)
        return MEM_NOT_ALLOC;

    result_msg = Test_file_write(mul_init, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        free(str);
        return result_msg;
    }

    for (int i = 0; i < test_size; i++)
    {
        result_msg = bi_get_random(&a, test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS || a->word_len != test_word_size)
            break;

        result_msg = bi_get_random(&b, test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS || b->word_len != test_word_size)
            break;

        if (bigint_to_hex(&a, str) == -1)
            break;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = Test_file_write_non_enter(" * ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        if (bigint_to_hex(&b, str) == -1)
            break;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = bi_mul(&c, &a, &b);
        if (result_msg != BI_MUL_SUCCESS)
            break;

        if (bigint_to_hex(&c, str) == -1)
            break;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = bi_delete(&a);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = bi_delete(&b);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = bi_delete(&c);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = Test_BI_MUL_SUCCESS;
    }

    free(str);
    if (bi_delete(&a) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    return result_msg;
}

msg test_bi_mul_karachuba(const IN int test_size, const IN int test_word_size)
{
    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_MUL_KARACHUBA_SUCCESS;

    str = (char *)calloc((test_word_size * 8) * 3 + 100, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
    if (str == NULL)
        return MEM_NOT_ALLOC;

    result_msg = Test_file_write(mul_karachuba_init, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        free(str);
        return result_msg;
    }

    for (int i = 0; i < test_size; i++)
    {
        result_msg = bi_get_random(&a, test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS || a->word_len != test_word_size)
            break;

        result_msg = bi_get_random(&b, test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS || b->word_len != test_word_size)
            break;

        if (bigint_to_hex(&a, str) == -1)
            break;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = Test_file_write_non_enter(" * ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        if (bigint_to_hex(&b, str) == -1)
            break;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = bi_mul_karachuba(&c, &a, &b);
        if (result_msg != BI_MUL_SUCCESS)
            break;

        if (bigint_to_hex(&c, str) == -1)
            break;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = bi_delete(&a);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = bi_delete(&b);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = bi_delete(&c);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = Test_BI_MUL_KARACHUBA_SUCCESS;
    }

    free(str);
    if (bi_delete(&a) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&b) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    return result_msg;
}

msg compare_multiplicaiton(int start_size, int end_size, int step_size, int iterations)
{
    printf("\n=== Comparing Multiplication Methods ===\n");
    printf("Size\titeration\tClassic(s)\tKaratsuba(s)\tRatio\tCrossover\n");
    printf("------------------------------------------------------------------------------\n");

    bigint *a = NULL;
    bigint *b = NULL;
    bigint *c = NULL;
    msg result_msg = COMPARE_MULTIPLICATION_SUCCESS;
    int crossover_found = 0;
    int crossover_point = 0;

    for (int test_word_size = start_size; test_word_size <= end_size; test_word_size += step_size)
    {
        double total_time_classic = 0;
        double total_time_karatsuba = 0;

        for (int i = 0; i < iterations; i++)
        {
            clock_t start, end;

            result_msg = bi_get_random(&a, test_word_size);
            if (result_msg != BI_GET_RANDOM_SUCCESS || a->word_len != test_word_size)
                goto COMAPARE_MUL_EXIT;

            result_msg = bi_get_random(&b, test_word_size);
            if (result_msg != BI_GET_RANDOM_SUCCESS || b->word_len != test_word_size)
                goto COMAPARE_MUL_EXIT;

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

            result_msg = bi_delete(&a);
            if (result_msg != BI_FREE_SUCCESS)
                break;

            result_msg = bi_delete(&b);
            if (result_msg != BI_FREE_SUCCESS)
                break;

            result_msg = bi_delete(&c);
            if (result_msg != BI_FREE_SUCCESS)
                break;
        }
        double avg_time_classic = total_time_classic / iterations;
        double avg_time_karatsuba = total_time_karatsuba / iterations;
        double ratio = avg_time_classic / avg_time_karatsuba;

        // Crossover point 찾기 (Karatsuba가 더 빨라지는 지점)
        if (!crossover_found && ratio > 1.0)
        {
            crossover_found = 1;
            crossover_point = test_word_size;
        }

        printf("%d\t%d\t\t%.6f\t%.6f\t%.2f\t%s\n",
               test_word_size,
               iterations,
               avg_time_classic,
               avg_time_karatsuba,
               ratio,
               (test_word_size == crossover_point) ? "<=== Crossover" : "");
    }

    if (crossover_found)
    {
        printf("\nKaratsuba becomes faster at word size: %d\n", crossover_point);
    }
    result_msg = COMPARE_MULTIPLICATION_SUCCESS;

COMAPARE_MUL_EXIT:
    if (bi_delete(&a) != BI_FREE_SUCCESS)
        log_msg(BI_FREE_FAIL);
    if (bi_delete(&b) != BI_FREE_SUCCESS)
        log_msg(BI_FREE_FAIL);
    if (bi_delete(&c) != BI_FREE_SUCCESS)
        log_msg(BI_FREE_FAIL);
    log_msg(result_msg);
    return result_msg;
}

msg test_bi_squ(const IN int test_size, const IN int test_word_size)
{
    bigint *a = NULL;
    bigint *c = NULL;
    char *str = NULL;
    msg result_msg = Test_BI_SQU_SUCCESS;

    str = (char *)calloc((test_word_size * 8) * 3 + 100, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
    if (str == NULL)
        return MEM_NOT_ALLOC;

    result_msg = Test_file_write(mul_init, APPEND);
    if (result_msg != FILE_WRITE_SUCCESS)
    {
        free(str);
        return result_msg;
    }

    for (int i = 0; i < test_size; i++)
    {
        result_msg = bi_get_random(&a, test_word_size);
        if (result_msg != BI_GET_RANDOM_SUCCESS || a->word_len != test_word_size)
            break;

        if (bigint_to_hex(&a, str) == -1)
            break;
        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = Test_file_write_non_enter(" * ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = Test_file_write_non_enter(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = Test_file_write_non_enter(" = ", APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = bi_squ(&c, &a);
        if (result_msg != BI_SQU_SUCCESS)
            break;

        if (bigint_to_hex(&c, str) == -1)
            break;
        result_msg = Test_file_write(str, APPEND);
        if (result_msg != FILE_WRITE_SUCCESS)
            break;

        result_msg = bi_delete(&a);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = bi_delete(&c);
        if (result_msg != BI_FREE_SUCCESS)
            break;

        result_msg = Test_BI_SQU_SUCCESS;
    }

    free(str);
    if (bi_delete(&a) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    if (bi_delete(&c) != BI_FREE_SUCCESS)
        return BI_FREE_FAIL;
    return result_msg;
}
// msg test_bi_div(const IN int test_size, const IN int test_word_size)
// {
//     bigint *q = NULL;
//     bigint *r = NULL;
//     bigint *a = NULL;
//     bigint *b = NULL;
//     char *str = NULL;
//     msg result_msg = Test_BI_DIV_SUCCESS;

//     str = (char *)calloc((test_word_size * 8) * 3 + 100, sizeof(char));
//     if (str == NULL)
//         return MEM_NOT_ALLOC;

//     result_msg = Test_file_write(div_init, APPEND);
//     if (result_msg != FILE_WRITE_SUCCESS)
//     {
//         free(str);
//         return result_msg;
//     }

//     for (int i = 0; i < test_size; i++)
//     {
//         // 메모리가 남아있을 수 있으므로 반복문 시작시 NULL로 초기화
//         q = NULL;
//         r = NULL;
//         a = NULL;
//         b = NULL;

//         result_msg = bi_get_random(&a, test_word_size);
//         if (result_msg != BI_GET_RANDOM_SUCCESS || a == NULL)
//             break;

//         result_msg = bi_get_random(&b, test_word_size);
//         if (result_msg != BI_GET_RANDOM_SUCCESS || b == NULL || bi_is_zero(b))
//         {
//             bi_delete(&a);
//             continue;
//         }

//         // a 출력
//         if (bigint_to_hex(&a, str) == -1)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             break;
//         }
//         result_msg = Test_file_write_non_enter(str, APPEND);
//         if (result_msg != FILE_WRITE_SUCCESS)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             break;
//         }

//         // 나누기 표시
//         result_msg = Test_file_write_non_enter(" / ", APPEND);
//         if (result_msg != FILE_WRITE_SUCCESS)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             break;
//         }

//         // b 출력
//         if (bigint_to_hex(&b, str) == -1)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             break;
//         }
//         result_msg = Test_file_write_non_enter(str, APPEND);
//         if (result_msg != FILE_WRITE_SUCCESS)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             break;
//         }

//         result_msg = Test_file_write_non_enter(" = ", APPEND);
//         if (result_msg != FILE_WRITE_SUCCESS)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             break;
//         }

//         // 나눗셈 수행
//         result_msg = bi_div(&q, &r, &a, &b);
//         if (result_msg != BI_DIV_SUCCESS)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             break;
//         }

//         // 몫 출력
//         if (bigint_to_hex(&q, str) == -1)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             bi_delete(&q);
//             bi_delete(&r);
//             break;
//         }
//         result_msg = Test_file_write_non_enter(str, APPEND);
//         if (result_msg != FILE_WRITE_SUCCESS)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             bi_delete(&q);
//             bi_delete(&r);
//             break;
//         }

//         // remainder 출력
//         result_msg = Test_file_write_non_enter(" remainder ", APPEND);
//         if (result_msg != FILE_WRITE_SUCCESS)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             bi_delete(&q);
//             bi_delete(&r);
//             break;
//         }

//         // 나머지 출력
//         if (bigint_to_hex(&r, str) == -1)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             bi_delete(&q);
//             bi_delete(&r);
//             break;
//         }
//         result_msg = Test_file_write(str, APPEND);
//         if (result_msg != FILE_WRITE_SUCCESS)
//         {
//             bi_delete(&a);
//             bi_delete(&b);
//             bi_delete(&q);
//             bi_delete(&r);
//             break;
//         }

//         // 할당된 메모리 해제
//         bi_delete(&a);
//         bi_delete(&b);
//         bi_delete(&q);
//         bi_delete(&r);

//         result_msg = Test_BI_DIV_SUCCESS;
//     }

//     // 문자열 버퍼 해제
//     free(str);

//     // 혹시 남아있을 수 있는 메모리 정리
//     if (a != NULL)
//         bi_delete(&a);
//     if (b != NULL)
//         bi_delete(&b);
//     if (q != NULL)
//         bi_delete(&q);
//     if (r != NULL)
//         bi_delete(&r);

//     return result_msg;
// }

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