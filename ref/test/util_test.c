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

    test_array = (word*)calloc(test_size, sizeof(word));
    if(test_array == NULL){
        return MEM_NOT_ALLOC;
    }

    for(int i = 0; i < test_size; i++){
        test_array[i] = rand() & 0xFFFFFFFF;
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

    // string set test
    // 추후 개발

    return Test_BI_SET_FROM_SUCCESS;
}