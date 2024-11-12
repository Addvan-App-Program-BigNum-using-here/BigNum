#ifndef MSG_CONTROL_H
#define MSG_CONTROL_H

#include <stdint.h>
#include "data_type.h"

#define MSG_TYPE_STANDARD   0x00000000    // 표준 에러
#define MSG_TYPE_MEMORY     0x00010000    // 메모리 에러
#define MSG_TYPE_STRING     0x00020000    // 문자열 에러
#define MSG_TYPE_IO         0x00030000    // 입출력 에러 및 랜덤
#define MSG_TYPE_TEST       0x00040000    // 테스트 에러
#define MSG_TYPE_OPERATE     0x00050000    // 연산 관련된 에러
#define MSG_TYPE_OTHER      0x00060000    // 그 외 에러
// 추가 에러 타입...

// 표준 메시지
enum StandardMSGCodes {
    SUCCESS = MSG_TYPE_STANDARD | 0x0001, // 성공
    FAIL    = MSG_TYPE_STANDARD | 0x0002, // 실패
    BI_INVALID_LENGTH = MSG_TYPE_STANDARD | 0x0003, // Bigint 길이가 0 이하인 경우
    SET_ARRAY_SIZE_FAIL = MSG_TYPE_STANDARD | 0x0004, // 배열 할당 시 길이가 0 이하인 경우
};

// 메모리 관련 에러
enum MemoryMSGCodes {
    BI_ALLOC_SUCCESS     = MSG_TYPE_MEMORY | 0x0001,  // Bigint 메모리 할당 성공
    BI_ALLOC_FAIL        = MSG_TYPE_MEMORY | 0x0002,  // Bigint 메모리 할당 실패
    BI_FREE_SUCCESS      = MSG_TYPE_MEMORY | 0x0003,  // Bigint 메모리 해제 성공
    BI_FREE_FAIL         = MSG_TYPE_MEMORY | 0x0004,  // Bigint 메모리 해제 실패
    BI_NOT_USING         = MSG_TYPE_MEMORY | 0x0005,  // 해제 시 NULL 체크를 하는데 이전에 사용 이력이 없는 경우
    MEM_NOT_ALLOC        = MSG_TYPE_MEMORY | 0x0006,  // Bigint 구조체가 아닌 다른 동적 할당에서 할당이 안될 경우
    BI_SET_ARRAY_SUCCESS = MSG_TYPE_MEMORY | 0x0007,  // Bigint 배열 할당 성공
    BI_SET_ARRAY_FAIL    = MSG_TYPE_MEMORY | 0x0008,  // Bigint 배열 할당 실패
    BI_SET_ASSIGN_SUCCESS= MSG_TYPE_MEMORY | 0x0009,  // Bigint Assign 성공
    BI_SET_ASSIGN_FAIL   = MSG_TYPE_MEMORY | 0x000A,  // Bigint Assign 실패
    BI_SET_REFINE_SUCCESS= MSG_TYPE_MEMORY | 0x000B,  // Bigint Refine 성공
    BI_SET_REFINE_FAIL   = MSG_TYPE_MEMORY | 0x000C,  // Bigint Refine 실패
    BI_EXPAND_SUCCESS    = MSG_TYPE_MEMORY | 0x000D,  // Bigint expand 성공
    BI_EXPAND_FAIL       = MSG_TYPE_MEMORY | 0x000E,  // Bigint expand 실패
    BI_SET_ARRAY_NOT_MATCH = MSG_TYPE_MEMORY | 0x000F, // Bigint 배열 할당 시 길이가 맞지 않는 경우
    BI_SHIFT_SUCCESS     = MSG_TYPE_MEMORY | 0x0010,  // Bigint shift 성공
    BI_SHIFT_FAIL        = MSG_TYPE_MEMORY | 0x0011,  // Bigint shift 실패
    BI_MUL_SUCCESS      = MSG_TYPE_MEMORY | 0x0012,  // Bigint 곱셈 성공
    BI_MUL_FAIL         = MSG_TYPE_MEMORY | 0x0013,  // Bigint 곱셈 실패
    BI_MOD_SUCCESS      = MSG_TYPE_MEMORY | 0x0014,  // Bigint 나머지 연산 성공
    BI_MOD_FAIL         = MSG_TYPE_MEMORY | 0x0015,  // Bigint 나머지 연산 실패
    // 추가 메모리 관련 에러 코드...
};

enum StringMSGCodes {
    DIVIDE_STRING_SUCCESS = MSG_TYPE_STRING | 0x0001, // 10진수 string Bigint 변환 시 문자열 나누기 성공
    DIVIDE_STRING_FAIL    = MSG_TYPE_STRING | 0x0002, // 10진수 string Bigint 변환 시 문자열 나누기 실패
    BI_SET_STRING_SUCCESS = MSG_TYPE_STRING | 0x0003, // String to Bigint 성공
    BI_SET_STRING_FAIL    = MSG_TYPE_STRING | 0x0004, // String to Binint 실패
    RAND_STRING_SUCCESS    = MSG_TYPE_STRING | 0x0005, // 랜덤 문자열 생성 성공
    RAND_STRING_INVALID   = MSG_TYPE_STRING | 0x0006, // 랜덤 문자열 생성 시 base가 2, 10이 아닌 경우
    BI_TO_HEX_FAIL        = MSG_TYPE_STRING | 0x0008, // Bigint to Hex 실패
    RAND_BYTE_SUCCESS     = MSG_TYPE_STRING | 0x0009, // 랜덤 바이트 생성 성공
    RAND_BYTE_FAIL        = MSG_TYPE_STRING | 0x000A, // 랜덤 바이트 생성 실패
    RAND_INIT_SUCCESS     = MSG_TYPE_STRING | 0x000B, // 랜덤 초기화 성공
    RAND_INIT_FAIL        = MSG_TYPE_STRING | 0x000C, // 랜덤 초기화 실패
    GEN_RANDOM_BYTES_SUCCESS = MSG_TYPE_STRING | 0x000D, // 랜덤 바이트 생성 성공
    GEN_RANDOM_BYTES_FAIL    = MSG_TYPE_STRING | 0x000E, // 랜덤 바이트 생성 실패
    // 추가 문자열 관련 에러 코드...
};

enum IOMSGCodes {
    GEN_RANDOM_SUCCESS        = MSG_TYPE_IO | 0x0001, // Bigint 랜덤 할당 성공
    GEN_RANDOM_FAIL           = MSG_TYPE_IO | 0x0002, // Bigint 랜덤 할당 실패
    PRINT_FAIL            = MSG_TYPE_IO | 0x0003, // print error
    PRINT_NULL            = MSG_TYPE_IO | 0x0004, // print NULL
    BI_GET_RANDOM_SUCCESS     = MSG_TYPE_IO | 0x0005, // Bigint 랜덤 할당 성공
    BI_GET_RANDOM_FAIL        = MSG_TYPE_IO | 0x0006, // Bigint 랜덤 할당 실패
    PRINT_SUCCESS            = MSG_TYPE_IO | 0x0007, // print success
    FILE_WRITE_SUCCESS        = MSG_TYPE_IO | 0x0008, // 파일 쓰기 성공
    FILE_WRITE_FAIL           = MSG_TYPE_IO | 0x0009, // 파일 쓰기 실패
    // 추가 입출력 관련 에러 코드...
};

enum TestMSGCodes {
    Test_FAIL                   = MSG_TYPE_TEST | 0x0001,      // Test 실패
    Test_SUCCESS                = MSG_TYPE_TEST | 0x0002,      // Test 성공
    Test_BI_NEW_DELETE_SUCCESS  = MSG_TYPE_TEST | 0x0003,      // Test Bigint 메모리 할당 및 해제 성공
    Test_BI_NEW_DELETE_FAIL     = MSG_TYPE_TEST | 0x0004,      // Test Bigint 메모리 할당 및 해제 실패
    Test_BI_SET_FROM_SUCCESS    = MSG_TYPE_TEST | 0x0005,      // Test Bigint 배열 및 문자열 할당 성공
    Test_BI_SET_FROM_FAIL       = MSG_TYPE_TEST | 0x0006,      // Test Bigint 배열 및 문자열 할당 실패
    Test_MEM_ALLOC_FAIL         = MSG_TYPE_TEST | 0x0007,      // Test 메모리 할당 실패
    Test_BI_GET_RANDOM_SUCCESS    = MSG_TYPE_TEST | 0x0008,      // Test Bigint 배열 및 문자열 할당 성공
    Test_BI_GET_RANDOM_FAIL     = MSG_TYPE_TEST | 0x0009,      // Test Bigint 랜덤 할당 실패
    Test_BI_ADD_SUCCESS     = MSG_TYPE_TEST | 0x000A,      // Test Bigint 연산 성공
    Test_BI_ADD_FAIL        = MSG_TYPE_TEST | 0x000B,      // Test Bigint 연산 실패
    Test_BI_SUB_SUCCESS     = MSG_TYPE_TEST | 0x000C,      // Test Bigint 연산 성공
    Test_BI_SUB_FAIL        = MSG_TYPE_TEST | 0x000D,      // Test Bigint 연산 실패
    Test_BI_MUL_SUCCESS     = MSG_TYPE_TEST | 0x000E,      // Test Bigint 연산 성공
    Test_BI_MUL_FAIL        = MSG_TYPE_TEST | 0x000F,      // Test Bigint 연산 실패
    Test_BI_SET_FROM_BASE_SUCCESS = MSG_TYPE_TEST | 0x0010, // Test Bigint 배열 및 문자열 할당 성공
    Test_BI_SET_FROM_BASE_FAIL    = MSG_TYPE_TEST | 0x0011, // Test Bigint 배열 및 문자열 할당 실패
    // 추가 테스트 관련 에러 코드...
};

enum OperateMSGCodes {
    BI_ADD_SUCCESS              = MSG_TYPE_OPERATE | 0x0001,    // Bigint 덧셈 성공
    BI_ADD_FAIL                 = MSG_TYPE_OPERATE | 0x0002,    // Bigint 덧셈 실패
    BI_SUB_SUCCESS              = MSG_TYPE_OPERATE | 0x0003,    // Bigint 뺄셈 성공
    BI_SUB_FAIL                 = MSG_TYPE_OPERATE | 0x0004,    // Bigint 뺄셈 실패
    // 추가 연산 관련 에러 코드...
};

// 에러 처리 함수 프로토타입
const char* get_msg_message(const IN uint32_t msg_code);
void log_msg(const IN uint32_t msg_code);

#endif // MSG_CONTROL_H