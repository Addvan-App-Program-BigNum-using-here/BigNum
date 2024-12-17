#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>
#include "data_type.h"
#include "msg_control.h"

#define Test_main_file "./test/test.txt"
#define Test_file_add "./test/test_vec/test_add.txt"
#define Test_file_sub "./test/test_vec/test_sub.txt"
#define Test_file_mul "./test/test_vec/test_mul.txt"
#define Test_file_mul_karachuba "./test/test_vec/test_mul_karachuba.txt"
#define Test_file_div "./test/test_vec/test_div.txt"
#define Test_file_squ "./test/test_vec/test_squ.txt"
#define Test_file_squ_karachuba "./test/test_vec/test_squ_karachuba.txt"
#define Test_file_exp "./test/test_vec/test_exp.txt"
#define Test_file_barrett_reduction "./test/test_vec/test_barrett_reduction.txt"
#define Test_file_gcd "./test/test_vec/test_gcd.txt"
#define Test_file_EEA "./test/test_vec/test_EEA.txt"
#define Test_file_MR "./test/test_vec/test_miller_rabin.txt"
#define Test_bigint "./test/test_vec/test_bigint.txt"
#define Test_file_RSA "./test/test_vec/test_RSA.txt"
#define Test_file_RSA_CRT "./test/test_vec/test_RSA_CRT.txt"

/**
 * @brief file write
 *
 * @param str string to write
 * @param option CLEAR or APPEND (CLEAR => clear file, APPEND => append file)
 * @return result of integer
 */
msg Test_file_write(IN char* Test_file, IN char* str, const IN int option);

/**
 * @brief file write without enter
 *
 * @param str string to write
 * @param option CLEAR or APPEND (CLEAR => clear file, APPEND => append file)
 * @return result of integer
 */
msg Test_file_write_non_enter(IN char* Test_file, IN char *str, const IN int option);

#endif // FILE_IO_H