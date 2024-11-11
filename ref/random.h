#ifndef RANDOM_H
#define RANDOM_H

#include "msg_control.h"
#include "util.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>

#ifdef _WIN32
    // Windows 시스템용 헤더
    #include <windows.h>
    #include <io.h>
    #include <wincrypt.h>
#else
    // Unix 계열(Linux, macOS) 시스템용 헤더
    #include <unistd.h>
#endif

/**
 * @brief New allocate bigint struct and fill it with random values
 *
 * @param dst pointer to bigint struct
 * @param word_len length of bigint struct
 */
msg bi_get_random(OUT bigint** dst, IN int word_len);

/**
 * @brief Fill word array with random values using for array_random()
 *
 * @param dst pointer to word array
 * @param word_len length of bigint struct
 */
msg array_random(OUT word* dst, IN int word_len);

///**
// * @brief Safe RAND_bytes function
// *
// * @param buffer pointer to uint8_t array
// * @param length random bytes length
// */
//msg secure_random_bytes(unsigned char* buffer, int length);

/**
 * @brief Maek bigint String using for get_random_string()
 *
 * @param str pointer to char array
 * @param str_len length of char array
 * @param base base of random string
 */
msg get_random_string(OUT char* str, IN int str_len, IN int base);

msg randombytes(IN byte* dst, IN int byte_len);

#endif // RANDOM_H