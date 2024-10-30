#include "error_msg.h"

static char *error_msg_list[6][10] = {{"SUCCESS", "FAIL"},
                                      {"BI_ALLOC_SUCCESS", "BI_ALLOC_FAIL", "BI_FREE_SUCCESS", "BI_FREE_FAIL", "BI_NOT_USING", "MEM_NOT_ALLOC", "BI_SET_ARRAY_SUCCESS", "BI_SET_ARRAY_FAIL"},
                                      {"GET_RANDOM_SUCCESS", "GET_RANDOM_FAIL", "GEN_RANDOM_SUCCESS", "GEN_RANDOM_FAIL", "BI_SET_REFINE_SUCCESS", "BI_SET_REFIEN_FAIL", "DIVIDE_STRING_SUCCESS", "DIVIDE_STRING_FAIL", "BI_SET_DIVIDE_STRING_SUCCESS", "BI_SET_DIVIDE_STRING_FAIL"},
                                      {"PRINT_FAIL", "PRINT_NULL"},
                                      {},
                                      {"Test_FAIL", "Test_SUCCESS", "Test_BI_NEW_DELETE_SUCCESS", "Test_BI_NEW_DELETE_FAIL", "Test_BI_SET_FROM_SUCCESS", "Test_BI_SET_FROM_FAIL", "Test_Bigint_ALLOC_FAIL"}};

void print_log(int error_code)
{
    unsigned int error_index = error_code / 10;
    unsigned int error_number = error_code % 10;

    if (error_code < 0 || error_index >= sizeof(error_msg_list) / sizeof(error_msg_list[0]) || error_msg_list[error_index][error_number] == NULL)
    {
        printf("Unknown error code %d\n", error_code);
    }
    else
    {
        printf("%s\n", error_msg_list[error_code / 10][error_code % 10]);
    }
}