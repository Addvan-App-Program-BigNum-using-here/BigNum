#include "util_test.h"

int main(){
    msg result_msg = 0;
    FILE *fp = NULL;
    int test_size = 1;
    int test_word_size = 13;
    char TEST_init[20] = "[TEST CASE START]";
    char TEST_end[20] = "[TEST CASE END]";

    Test_file_write(TEST_init, CLEAR);

    result_msg = test_bi_new_delete();
    log_msg(result_msg);
    if(result_msg != Test_BI_NEW_DELETE_SUCCESS){
        return Test_FAIL;
    }

    result_msg = test_bi_set_from(test_size);
    log_msg(result_msg);
    if(result_msg != Test_BI_SET_FROM_SUCCESS){
        return Test_FAIL;
    }

    result_msg = test_bi_random(test_size, test_word_size);
    log_msg(result_msg);
    if(result_msg != Test_BI_GET_RANDOM_SUCCESS){
        return Test_BI_GET_RANDOM_FAIL;
    }

    result_msg = test_bi_add(test_size, test_word_size);
    log_msg(result_msg);
    if(result_msg != Test_BI_ADD_SUCCESS){
        return Test_BI_ADD_FAIL;
    }

    result_msg = test_bi_sub(test_size, test_word_size);
    log_msg(result_msg);
    if(result_msg != Test_BI_SUB_SUCCESS){
        return Test_BI_SUB_FAIL;
    }

    Test_file_write(TEST_end, APPEND);

    // Sage test
    fp = popen("sage ../../sage_test/test.sage >/dev/null 2>&1", "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return 1;
    }

    // Close the command stream
    int status = pclose(fp);
    if(status == -1){
        perror("pclose failed");
        return 1;
    } else {
        printf("python script exited with status %d\n", status);
    }

    return 0;
}

msg test_bi_new_delete(){
    bigint* a = NULL;
    msg result_msg = 0;

    result_msg = bi_new(&a, 4);
    if(result_msg == BI_ALLOC_FAIL || a->word_len != 4){
        return result_msg;
    }

    result_msg = bi_delete(&a);
    if(result_msg == BI_FREE_FAIL){
        return result_msg;
    }

    return Test_BI_NEW_DELETE_SUCCESS;
}

msg test_bi_set_from(int test_size){
    bigint* a = NULL;
    word* test_array = NULL;
    char* str = NULL; // bigint를 16진수로 변환한 문자열
    char* str_2 = NULL; // 2진수 문자열
    char* str_10 = NULL; // 10진수 문자열
    char* str_16 = NULL; // 16진수 문자열
    char FROM_init[20] = "\n[BI SET FROM]";
    int array_size = 0, result_msg = 0;

    Test_file_write(FROM_init, APPEND);

    for(int i = 0; i < test_size; i++){
        array_size = rand() % 1000 + 1;
        test_array = (word*)calloc(array_size, sizeof(word));

        if(test_array == NULL)  return MEM_NOT_ALLOC;
        array_random(test_array, array_size); // 랜덤한 배열 생성

        // little endian set test
        result_msg = bi_set_from_array(&a, 1, array_size, test_array, little_endian);
        if(result_msg != BI_SET_ARRAY_SUCCESS){
            log_msg(result_msg);
            if(bi_delete(&a) != BI_FREE_SUCCESS)   return result_msg;
            return result_msg;
        }

        for(int i = 0; i < array_size; i++){
            if(a->a[i] != test_array[i]){
                printf("Data NOT MATCH little endian version\n");
                return Test_BI_SET_FROM_FAIL;
            }
        }

        result_msg = bi_delete(&a);
        if(result_msg != BI_FREE_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }

        // big_endian set test
        result_msg = bi_set_from_array(&a, 1, array_size, test_array, big_endian);
        if(result_msg != BI_SET_ARRAY_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }

        for(int i = 0; i < array_size; i++){
            if(a->a[(array_size - 1) - i] != test_array[i]){
                printf("Data NOT MATCH big endian version\n");
                return Test_BI_SET_FROM_FAIL;
            }
        }

        result_msg = bi_delete(&a);
        if(result_msg != BI_FREE_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }

        free(test_array);

        //************************************ string set test *************************************


        // 테스트 str 초기화
        str = (char*)calloc(array_size + 1, sizeof(char));
        str_2 = (char*)calloc(array_size + 1, sizeof(char));
        str_10 = (char*)calloc(array_size + 1, sizeof(char));
        str_16 = (char*)calloc(array_size + 1, sizeof(char));

        // 랜덤한 str 가져오기
        get_random_string(str_2, array_size - 1, 2);
//        get_random_string(str_10, array_size - 1, 10); // 10진수는 일단 예외
        get_random_string(str_16, array_size - 1, 16);

        // 2진수 테스트
        Test_file_write("[2]", APPEND); // 줄바꿈
        Test_file_write(str_2, APPEND); // 줄바꿈
        result_msg = bi_set_from_string(&a, str_2, 2); // 2진수 테스트
        if(result_msg != BI_SET_STRING_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }

        bigint_to_hex(&a, str); // bigint를 16진수로 변환
        Test_file_write(str, APPEND); // 변환한 16진수 문자열을 txt에 넣는다.

        if(bi_delete(&a) != BI_FREE_SUCCESS)    return Test_BI_SET_FROM_FAIL;

/*
        // 10진수 테스트
        Test_file_write("[10]", APPEND); // 줄바꿈
        Test_file_write(str_10, APPEND); // 줄바꿈
        result_msg = bi_set_from_string(&a, str_10, 10); // 10진수 테스트
        if(result_msg != BI_SET_STRING_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }
        bigint_to_hex(&a, str); // bigint를 16진수로 변환
        Test_file_write(str, APPEND); // 변환한 16진수 문자열을 txt에 넣는다.

        if(bi_delete(&a) != BI_FREE_SUCCESS){
            return Test_BI_SET_FROM_FAIL;
        }
*/
        // 16진수 테스트
        Test_file_write("[16]", APPEND); // 줄바꿈
        Test_file_write(str_16, APPEND); // 줄바꿈
        result_msg = bi_set_from_string(&a, str_16, 16); // 16진수 테스트
        if(result_msg != BI_SET_STRING_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }
        bigint_to_hex(&a, str); // bigint를 16진수로 변환
        Test_file_write(str, APPEND); // 변환한 16진수 문자열을 txt에 넣는다.

        if(bi_delete(&a) != BI_FREE_SUCCESS){
            return Test_BI_SET_FROM_FAIL;
        }

        free(str);
        free(str_2);
        free(str_10);
        free(str_16);
    }

    return Test_BI_SET_FROM_SUCCESS;
}

msg test_bi_random(const IN int test_size, const IN int test_word_size){
    bigint* dst = NULL;
    msg result_msg = 0;

    for(int i = 0 ; i < test_size; i++){
        result_msg = bi_get_random(&dst, test_word_size);
        if(result_msg != BI_GET_RANDOM_SUCCESS ){
            log_msg(result_msg);
            return Test_BI_GET_RANDOM_FAIL;
        }
        bi_delete(&dst);
    }

    return Test_BI_GET_RANDOM_SUCCESS;
}


msg test_bi_add(const IN int test_size, const IN int test_word_size){
    bigint* a = NULL;
    bigint* b = NULL;
    bigint* c = NULL;
    char add_init[12] = "\n[Addition]";
    char* str = NULL;
    msg result_msg = 0;

    str = (char*)calloc(test_word_size * 24 + 12, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
    if(str == NULL) return MEM_NOT_ALLOC;
    Test_file_write(add_init, APPEND);

    for(int i = 0; i < test_size; i++){
        result_msg = bi_get_random(&a, test_word_size);
        if(result_msg == BI_ALLOC_FAIL || a->word_len != test_word_size){
            return result_msg;
        }

        result_msg = bi_get_random(&b, test_word_size);
        if(result_msg == BI_ALLOC_FAIL || b->word_len != test_word_size){
            return result_msg;
        }

        result_msg = bi_add(&c, &a, &b);
        if(result_msg != BI_ADD_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }

        operate_string_cat(str, &a, &b, &c, '+'); // a + b = c을 문자열로 변환
        Test_file_write(str, APPEND); // 파일에 문자열 저장

        result_msg = bi_delete(&a);
        if(result_msg != BI_FREE_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }

        result_msg = bi_delete(&b);
        if(result_msg != BI_FREE_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }

        result_msg = bi_delete(&c);
        if(result_msg != BI_FREE_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }
    }
    free(str);
    return Test_BI_ADD_SUCCESS;
}

msg test_bi_sub(const IN int test_size, const IN int test_word_size){
    bigint* a = NULL;
    bigint* b = NULL;
    bigint* c = NULL;
    char sub_init[20] = "\n[Subtraction]";
    char* str = NULL;
    msg result_msg = 0;

    str = (char*)calloc(test_word_size * 24 + 12, sizeof(char)); // 12는 0x문자열과 operator, =, \n,\n을 위한 공간
    if(str == NULL) return MEM_NOT_ALLOC;

    Test_file_write(sub_init, APPEND);

    for(int i = 0; i < test_size; i++){
        result_msg = bi_get_random(&a, test_word_size);
        if(result_msg == BI_ALLOC_FAIL || a->word_len != test_word_size){
            return result_msg;
        }

        result_msg = bi_get_random(&b, test_word_size);
        if(result_msg == BI_ALLOC_FAIL || b->word_len != test_word_size){
            return result_msg;
        }

        result_msg = bi_sub(&c, &a, &b);
        if(result_msg != BI_SUB_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }

        operate_string_cat(str, &a, &b, &c, '-'); // a + b = c을 문자열로 변환
        Test_file_write(str, APPEND); // 파일에 문자열 저장


        result_msg = bi_delete(&a);
        if(result_msg != BI_FREE_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }

        result_msg = bi_delete(&b);
        if(result_msg != BI_FREE_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }

        result_msg = bi_delete(&c);
        if(result_msg != BI_FREE_SUCCESS){
            log_msg(result_msg);
            return result_msg;
        }
    }

    free(str);

    return Test_BI_SUB_SUCCESS;

}