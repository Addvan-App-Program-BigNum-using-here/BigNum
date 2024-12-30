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
 * @return message  SUCCESS or FAIL
 */
msg bi_get_random(OUT bigint **dst, IN int word_len);

/**
 * @brief New allocate bigints struct and fill it with random values
 *
 * @param count bigint count
 * @param ... bigint** dst
 * @return message  SUCCESS or FAIL
 */
msg get_random_bigint(int random_word_size, int word_size, int word_size_limit, int count, ...);

/**
 * @brief Fill word array with random values
 *
 * @param dst pointer to word array
 * @param word_len length of bigint struct
 * @return message SUCCESS or FAIL
 */
msg array_random(OUT word *dst, IN int word_len);

/**
 * @brief Make bigint String
 *
 * @param g pointer to char array
 * @param str_len length of char array
 * @param base base of random string
 * @return message SUCCESS or FAIL
 */
msg get_random_string(OUT char** str, IN int str_len, IN int base);

/**
 * @brief get random byte array
 *
 * @param dst byte array of random values
 * @param byte_len length of byte array
 * @return message SUCCESS or FAIL
 */
msg randombytes(IN byte *dst, IN int byte_len);

#endif // RANDOM_H