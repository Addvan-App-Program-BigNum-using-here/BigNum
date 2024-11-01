#include "util_test.h"

int main(){
    msg result_msg = 0;
    FILE *fp = NULL;
    int test_size = 1;

//    result_msg = test_bi_new_delete();
//    log_msg(result_msg);
//    if(result_msg != Test_BI_NEW_DELETE_SUCCESS){
//        return Test_FAIL;
//    }
//
//    result_msg = test_bi_set_from();
//    log_msg(result_msg);
//    if(result_msg != Test_BI_SET_FROM_SUCCESS){
//        return Test_FAIL;
//    }
//
//    result_msg = test_bi_random();
//    log_msg(result_msg);
//    if(result_msg != Test_BI_GET_RANDOM_SUCCESS){
//        return Test_BI_GET_RANDOM_FAIL;
//    }
//
    printf("----------- add test --------------\n");
    result_msg = test_bi_add(test_size);
    log_msg(result_msg);
    if(result_msg != Test_BI_ADD_SUCCESS){
        return Test_BI_ADD_FAIL;
    }

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

msg test_bi_set_from(){
    bigint* a = NULL;
    word* test_array = NULL;
    int test_size = rand() % 100 + 1;
    int result_msg = 0;

    char test_string[3][60] = {"101001010101101010111110101111001011010110101000", "123456789090909090908080", "1234567890abcdef"};

    test_array = (word*)calloc(test_size, sizeof(word));
    if(test_array == NULL){
        return MEM_NOT_ALLOC;
    }

    for(int i = 0; i < test_size; i++){
        for (int j = 0; j < 4; j++) {
            test_array[i] += (rand() & 0xff) << (8 * j);
        }
    }

    // little endian set test
    result_msg = bi_set_from_array(&a, 1, test_size, test_array, little_endian);
    if(result_msg != BI_SET_ARRAY_SUCCESS){
        log_msg(result_msg);
        return result_msg;
    }

    for(int i = 0; i < test_size; i++){
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
    result_msg = bi_set_from_array(&a, 1, test_size, test_array, big_endian);
    if(result_msg != BI_SET_ARRAY_SUCCESS){
        log_msg(result_msg);
        return result_msg;
    }

    for(int i = 0; i < test_size; i++){
        if(a->a[(test_size - 1) - i] != test_array[i]){
            printf("Data NOT MATCH big endian version\n");
            return Test_BI_SET_FROM_FAIL;
        }
    }

    result_msg = bi_delete(&a);
    if(result_msg != BI_FREE_SUCCESS){
        log_msg(result_msg);
        return result_msg;
    }

    //************************************ string set test *************************************

    // 2진수 테스트
    result_msg = bi_set_from_string(&a, test_string[0], 2); // 2진수 테스트
    if(result_msg != BI_SET_STRING_SUCCESS){
        log_msg(result_msg);
        return result_msg;
    }

    printf("**** 2진수 테스트 ****\n");
    bi_print(&a, 16);

    if(bi_delete(&a) != BI_FREE_SUCCESS){
        return Test_BI_SET_FROM_FAIL;
    }

    // 10진수 테스트
    result_msg = bi_set_from_string(&a, test_string[1], 10); // 10진수 테스트
    if(result_msg != BI_SET_STRING_SUCCESS){
        log_msg(result_msg);
        return result_msg;
    }

    printf("**** 10진수 테스트 ****\n");
    bi_print(&a, 16);

    if(bi_delete(&a) != BI_FREE_SUCCESS){
        return Test_BI_SET_FROM_FAIL;
    }

    // 16진수 테스트
    result_msg = bi_set_from_string(&a, test_string[2], 16); // 16진수 테스트
    if(result_msg != BI_SET_STRING_SUCCESS){
        log_msg(result_msg);
        return result_msg;
    }

    printf("**** 16진수 테스트 ****\n");
    bi_print(&a, 16);

    if(bi_delete(&a) != BI_FREE_SUCCESS){
        return Test_BI_SET_FROM_FAIL;
    }

    free(test_array);

    return Test_BI_SET_FROM_SUCCESS;
}

msg test_bi_random(){
    bigint* dst = NULL;
    msg result_msg = 0;
    int word_len = rand() % 65;
    result_msg = bi_get_random(&dst, word_len);
    log_msg(result_msg);
    if(result_msg != BI_GET_RANDOM_SUCCESS ){
        log_msg(result_msg);
        return Test_BI_GET_RANDOM_FAIL;
    }

    bi_print(&dst, 16);

    bi_delete(&dst);

    return Test_BI_GET_RANDOM_SUCCESS;
}


msg test_bi_add(int test_size){
    bigint* a = NULL;
    bigint* b = NULL;
    bigint* c = NULL;
    char add_init[12] = "[Addition]";
    char str[1024];
    msg result_msg = 0;
    int test_word_size = 8;

    Test_file_write(add_init, CLEAR);

    for(int i = 0; i < test_size; i++){
        result_msg = bi_get_random(&a, test_word_size);
        if(result_msg == BI_ALLOC_FAIL || a->word_len != test_word_size){
            return result_msg;
        }

        result_msg = bi_get_random(&b, test_word_size);
        if(result_msg == BI_ALLOC_FAIL || b->word_len != test_word_size){
            return result_msg;
        }

        result_msg = bi_get_random(&c, test_word_size);
        if(result_msg == BI_ALLOC_FAIL || c->word_len != test_word_size){
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

    return Test_BI_ADD_SUCCESS;
}
    
