#include "msg_control.h"
#include <stdio.h>

void log_msg(const IN uint32_t msg_code)
{
    const char *msg_type = "UNKNOWN";
    if ((msg_code & MSG_TYPE_STANDARD) == MSG_TYPE_STANDARD)
    {
        msg_type = "STANDARD";
    }
    else if ((msg_code & MSG_TYPE_MEMORY) == MSG_TYPE_MEMORY)
    {
        msg_type = "MEMORY";
    }
    else if ((msg_code & MSG_TYPE_STRING) == MSG_TYPE_STRING)
    {
        msg_type = "STRING";
    }
    else if ((msg_code & MSG_TYPE_IO) == MSG_TYPE_IO)
    {
        msg_type = "IO";
    }
    else if ((msg_code & MSG_TYPE_TEST) == MSG_TYPE_TEST)
    {
        msg_type = "TEST";
    }
    else if ((msg_code & MSG_TYPE_OPERATE) == MSG_TYPE_OPERATE)
    {
        msg_type = "OPERATE";
    }
    else if ((msg_code & MSG_TYPE_OTHER) == MSG_TYPE_OTHER)
    {
        msg_type = "OTHER";
    }
    // 추가 에러 타입에 대한 처리...

    fprintf(stderr, "[LOG] : [%s] 0x%08X : [%s]\n", msg_type, msg_code, get_msg_message(msg_code));
}

#include "msg_control.h"

const char *get_msg_message(const IN uint32_t msg_code)
{
    switch (msg_code)
    {
    case SUCCESS:
        return "Operation successful";
    case FAIL:
        return "Operation failed";
    case BI_ALLOC_SUCCESS:
        return "Bigint memory allocation successful";
    case BI_ALLOC_FAIL:
        return "Bigint memory allocation failed";
    case BI_FREE_SUCCESS:
        return "Bigint memory deallocation successful";
    case BI_FREE_FAIL:
        return "Bigint memory deallocation failed";
    case BI_NOT_USING:
        return "Bigint not in use";
    case MEM_NOT_ALLOC:
        return "Memory not allocated";
    case BI_SET_ARRAY_SUCCESS:
        return "Bigint array allocation successful";
    case BI_SET_ARRAY_FAIL:
        return "Bigint array allocation failed";
    case BI_SET_ASSIGN_SUCCESS:
        return "Bigint assignment successful";
    case BI_SET_ASSIGN_FAIL:
        return "Bigint assignment failed";
    case BI_SET_REFINE_SUCCESS:
        return "Bigint refinement successful";
    case BI_SET_REFINE_FAIL:
        return "Bigint refinement failed";
    case DIVIDE_STRING_SUCCESS:
        return "String division successful";
    case DIVIDE_STRING_FAIL:
        return "String division failed";
    case BI_SET_STRING_SUCCESS:
        return "String to Bigint conversion successful";
    case BI_SET_STRING_FAIL:
        return "String to Bigint conversion failed";
    case GEN_RANDOM_SUCCESS:
        return "Random Bigint generation successful";
    case GEN_RANDOM_FAIL:
        return "Random Bigint generation failed";
    case PRINT_FAIL:
        return "Print error";
    case PRINT_NULL:
        return "Print null";
    case Test_FAIL:
        return "Test failed";
    case Test_SUCCESS:
        return "Test successful";
    case Test_BI_NEW_DELETE_SUCCESS:
        return "Test Bigint memory allocation and deallocation successful";
    case Test_BI_NEW_DELETE_FAIL:
        return "Test Bigint memory allocation and deallocation failed";
    case Test_BI_SET_FROM_SUCCESS:
        return "Test Bigint array and string allocation successful";
    case Test_BI_SET_FROM_FAIL:
        return "Test Bigint array and string allocation failed";
    case Test_MEM_ALLOC_FAIL:
        return "Test memory allocation failed";
    case BI_ADD_SUCCESS:
        return "Bigint addition successful";
    case BI_ADD_FAIL:
        return "Bigint addition failed";
    case BI_EXPAND_SUCCESS:
        return "Bigint expansion successful";
    case BI_EXPAND_FAIL:
        return "Bigint expansion failed";
    case BI_GET_RANDOM_SUCCESS:
        return "Bigint random allocation successful";
    case BI_GET_RANDOM_FAIL:
        return "Bigint random allocation failed";
    case Test_BI_GET_RANDOM_SUCCESS:
        return "Test Bigint random allocation successful";
    case Test_BI_GET_RANDOM_FAIL:
        return "Test Bigint random allocation failed";
    case Test_BI_ADD_SUCCESS:
        return "Test Bigint Addition successful";
    case Test_BI_ADD_FAIL:
        return "Test Bigint Addition failed";
    case PRINT_SUCCESS:
        return "Print successful";
    case FILE_WRITE_SUCCESS:
        return "File write successful";
    case FILE_WRITE_FAIL:
        return "File write failed";
    case RAND_STRING_SUCCESS:
        return "Random string generation successful";
    case RAND_STRING_INVALID:
        return "Invalid random string";
    case BI_SUB_SUCCESS:
        return "Bigint subtraction successful";
    case BI_SUB_FAIL:
        return "Bigint subtraction failed";
    case Test_BI_SUB_SUCCESS:
        return "Test Bigint subtraction successful";
    case Test_BI_SUB_FAIL:
        return "Test Bigint subtraction failed";
    case BI_SET_ARRAY_NOT_MATCH:
        return "Bigint array allocation length mismatch";
    case BI_TO_HEX_FAIL:
        return "Bigint to Hex conversion failed";
    case BI_INVALID_LENGTH:
        return "Invalid length";
    case RAND_BYTE_SUCCESS:
        return "Random byte generation successful";
    case RAND_BYTE_FAIL:
        return "Random byte generation failed";
    case RAND_INIT_SUCCESS:
        return "Random initialization successful";
    case RAND_INIT_FAIL:
        return "Random initialization failed";
    case GEN_RANDOM_BYTES_SUCCESS:
        return "Random byte generation successful";
    case GEN_RANDOM_BYTES_FAIL:
        return "Random byte generation failed";
    case BI_SHIFT_SUCCESS:
        return "Bigint shift successful";
    case BI_SHIFT_FAIL:
        return "Bigint shift failed";
    case Test_BI_MUL_SUCCESS:
        return "Test Bigint multiplication successful";
    case Test_BI_MUL_FAIL:
        return "Test Bigint multiplication failed";
    case BI_MUL_SUCCESS:
        return "Bigint multiplication successful";
    case BI_MUL_FAIL:
        return "Bigint multiplication failed";
    case SET_ARRAY_SIZE_FAIL:
        return "Bigint array size set failed";
    case Test_BI_SET_FROM_BASE_SUCCESS:
        return "Test Bigint array and string allocation with base successful";
    case Test_BI_SET_FROM_BASE_FAIL:
        return "Test Bigint array and string allocation with base failed";
    case BI_MOD_SUCCESS:
        return "Bigint modulo operation successful";
    case BI_MOD_FAIL:
        return "Bigint modulo operation failed";
    case Test_BI_MUL_KARACHUBA_SUCCESS:
        return "Test Bigint Karachuba multiplication successful";
    case Test_BI_MUL_KARACHUBA_FAIL:
        return "Bigint refinement successful";
    case BI_DIV_SUCCESS:
        return "Bigint division successful";
    case BI_DIV_FAIL:
        return "Bigint division failed";
    case Test_BI_DIV_SUCCESS:
        return "Test Bigint division successful";
    case Test_BI_DIV_FAIL:
        return "Test Bigint division failed";
    case COMPARE_MULTIPLICATION_SUCCESS:
        return "Compare multiplication successful";
    case COMPARE_MULTIPLICATION_FAIL:
        return "Compare multiplication failed";
    case BI_CAT_SUCCESS:
        return "Bigint concatenation successful";
    case BI_CAT_FAIL:
        return "Bigint concatenation failed";
    case BI_SIGN_NOT_MATCH:
        return "Bigint sign mismatch";
    default:
        return "Unknown error code";
    }
}