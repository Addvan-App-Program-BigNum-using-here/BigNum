#include "util_test.h"

int main(){
    msg result_msg = 0;

    result_msg = test_bi_new_delete();
    print_log(result_msg);
    if(result_msg != Test_BI_NEW_DELETE_SUCCESS){
        return Test_FAIL;
    }

    result_msg = test_bi_set_from();
    print_log(result_msg);
    if(result_msg != Test_BI_SET_FROM_SUCCESS){
        return Test_FAIL;
    }

    return Test_SUCCESS;
}

/*************************************************
* Name:        test_bi_new_delete
*
* Description: Test bigint structure allocation and deallocation
**************************************************/
int test_bi_new_delete(){
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


/*************************************************
* Name:        test_bi_set_from
*
* Description: Test bigint structure set content from array and string
**************************************************/
int test_bi_set_from(){
    bigint* a = NULL;
    word* test_array = NULL;
    int test_size = rand() % 100 + 1;
    int result_msg = 0;

    char test_string[3][60] = {"0b101001010101101010111110101111001011010110101000", "123456789090909090908080", "0x1234567890abcdef"};

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
        print_log(result_msg);
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
        print_log(result_msg);
        return result_msg;
    }

    // big_endian set test
    result_msg = bi_set_from_array(&a, 1, test_size, test_array, big_endian);
    if(result_msg != BI_SET_ARRAY_SUCCESS){
        print_log(result_msg);
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
        print_log(result_msg);
        return result_msg;
    }

    //************************************ string set test *************************************

    // 2진수 테스트
    result_msg = bi_set_from_string(&a, test_string[0], 2); // 2진수 테스트
    if(result_msg != BI_SET_STRING_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }

    printf("**** 2진수 테스트 ****\n");
    for(int i = 0; i < a->word_len; i++){
        printf("0x%08x\n", a->a[i]);
    }

    if(bi_delete(&a) != BI_FREE_SUCCESS){
        return Test_BI_SET_FROM_FAIL;
    }

    // 10진수 테스트
    result_msg = bi_set_from_string(&a, test_string[1], 10); // 10진수 테스트
    if(result_msg != BI_SET_STRING_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }

    printf("**** 10진수 테스트 ****\n");
    for(int i = 0; i < a->word_len; i++){
        printf("0x%08x\n", a->a[i]);
    }

    if(bi_delete(&a) != BI_FREE_SUCCESS){
        return Test_BI_SET_FROM_FAIL;
    }

    // 16진수 테스트
    result_msg = bi_set_from_string(&a, test_string[2], 16); // 16진수 테스트
    if(result_msg != BI_SET_STRING_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }

    printf("**** 16진수 테스트 ****\n");
    for(int i = 0; i < a->word_len; i++){
        printf("0x%08x\n", a->a[i]);
    }

    if(bi_delete(&a) != BI_FREE_SUCCESS){
        return Test_BI_SET_FROM_FAIL;
    }

    return Test_BI_SET_FROM_SUCCESS;
}