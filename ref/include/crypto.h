#ifndef CRYPTO_H
#define CRYPTO_H

#include "data_type.h"
#include "util.h"
#include "msg_control.h"
#include "operate.h"

/**
 * @brief bigint structure RSA operation
 *
 * @return message SUCCESS or FAIL
 */
msg RSA(OUT bigint** m2, IN bigint** m);

/**
 * @brief bigint structure RSA CRT operation
 *
 * @return message SUCCESS or FAIL
 */
msg RSA_CRT(OUT bigint** m2, IN bigint** m);

/**
 * @brief bigint structure RSA KeyGen operation
 *
 * @return message SUCCESS or FAIL
 */
msg RSA_KeyGEN(OUT bigint** n, OUT bigint** e, OUT bigint** d);

/**
 * @brief bigint structure RSA Encryption operation
 *
 * @return message SUCCESS or FAIL
 */
msg RSA_ENC(OUT bigint** c, IN bigint** m, IN bigint** e, IN bigint** N);

/**
 * @brief bigint structure RSA Decryption operation
 *
 * @return message SUCCESS or FAIL
 */
msg RSA_DEC(OUT bigint** m, IN bigint** c, IN bigint** d, IN bigint** N);

/**
 * @brief bigint structure RSA CRT KeyGen operation
 *
 * @return message SUCCESS or FAIL
 */
msg RSA_CRT_KeyGEN(OUT bigint**n, OUT bigint** p, OUT bigint** q, OUT bigint** e, OUT bigint** d_p, OUT bigint** d_q, OUT bigint** q_inv);

/**
 * @brief bigint structure RSA CRT Decryption operation
 *
 * @return message SUCCESS or FAIL
 */
msg RSA_CRT_DEC(OUT bigint** m, IN bigint** n, IN bigint** c, IN bigint** p, IN bigint** q, IN bigint** d_p, IN bigint** d_q, IN bigint** q_inv);

/**
 * @brief bigint structure Miller-Rabin primality test
 * @param dst pointer to square of bigint
 * @param a dividend bigint to be squared
 * @return msg
 */
msg miller_rabin_primality(OUT bigint** temp, IN bigint **n, IN int k);

/**
 * @brief bigint structure gcd operation
 * @param dst pointer to square of bigint
 * @param a dividend bigint to be squared
 * @return msg
 */
msg bi_gcd(OUT bigint** dst, IN bigint** a, IN bigint** b);

/**
 * @brief bigint structure Extended Euclidean Algorithm
 * @param dst pointer to square of bigint
 * @param a dividend bigint to be squared
 * @return msg
 */
msg bi_EEA(OUT bigint** gcd, OUT bigint** x, OUT bigint** y, IN bigint** a, IN bigint** b);
#endif