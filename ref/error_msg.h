#include <stdio.h>

// 그외 에러 메시지 : 0번대
#define SUCCESS 0 // 성공
#define FAIL 1    // 실패

// MEMORY 할당 및 해제 관련 에러 메시지 : 10번대
#define BI_ALLOC_SUCCESS 10     // Bigint 메모리 할당 성공
#define BI_ALLOC_FAIL 11        // Bigint 메모리 할당 실패
#define BI_FREE_SUCCESS 12      // Bigint 메모리 해제 성공
#define BI_FREE_FAIL 13         // Bigint 메모리 해제 실패
#define BI_NOT_USING 14         // 해제 시 NULL 체크를 하는데 이전에 사용 이력이 없는 경우
#define MEM_NOT_ALLOC 15        // Bigint 구조체가 아닌 다른 동적할당에서 할당이 안될 경우
#define BI_SET_ARRAY_SUCCESS 16 // Bigint 배열 할당 성공
#define BI_SET_ARRAY_FAIL 17    // Bigint 배열 할당 실패
#define BI_SET_ASSIGN_SUCCESS 18
#define BI_SET_ASSIGN_FAIL 19

// Random 관련 에러 메시지 : 20번대
#define RANDOM_SUCCESS 20 // Bigint 랜덤 할당 성공
#define RANDOM_FAIL 21    // Bigint 랜덤 할당 실패

// Print 관련 에러 메시지 : 30번대
#define PRINT_FAIL 30 // Bigint 출력 실패
#define PRINT_NULL 31 // 출력하려는 Bigint 구조체가 NULL인 경우

// Util Test 관련 에러 메시지 : 50번대
#define Test_FAIL 50
#define Test_SUCCESS 51
#define Test_BI_NEW_DELETE_SUCCESS 52
#define Test_BI_NEW_DELETE_FAIL 53
#define Test_BI_SET_FROM_SUCCESS 54
#define Test_BI_SET_FROM_FAIL 55
#define Test_MEM_ALLOC_FAIL 56

// Error Control Function
void print_log(int error_code);