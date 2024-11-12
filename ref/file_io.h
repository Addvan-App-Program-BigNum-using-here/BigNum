#include <stdio.h>
#include "data_type.h"
#include "msg_control.h"

#define Test_file "./test.txt"

// 주석 추가하기
msg Test_file_write(IN char* str, IN int option);

msg Test_file_write_non_enter(IN char *str, IN int option);