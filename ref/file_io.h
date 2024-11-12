#include <stdio.h>
#include "data_type.h"
#include "msg_control.h"

#define Test_file "./test.txt"

/**
 * @brief file write
 *
 * @param str string to write
 * @param option CLEAR or APPEND (CLEAR => clear file, APPEND => append file)
 * @return result of integer
 */
msg Test_file_write(IN char* str, const IN int option);

/**
 * @brief file write without enter
 *
 * @param str string to write
 * @param option CLEAR or APPEND (CLEAR => clear file, APPEND => append file)
 * @return result of integer
 */
msg Test_file_write_non_enter(IN char *str, const IN int option);