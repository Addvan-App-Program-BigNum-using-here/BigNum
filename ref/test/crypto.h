#ifndef CRYPTO_H
#define CRYPTO_H

#include "../data_type.h"
#include "../util.h"
#include "../msg_control.h"
#include "../operate.h"

#define BITS_SIZE 512  ///< Test bit size, representing the RSA key length.

/**
 * @brief Performs RSA encryption and decryption operations.
 * @return Message indicating success or failure.
 */
msg RSA();

/**
 * @brief Performs RSA encryption and decryption using the Chinese Remainder Theorem (CRT).
 * @return Message indicating success or failure.
 */
msg RSA_CRT();

/**
 * @brief Generates RSA keys.
 * @param[out] n Public key N (modulus).
 * @param[out] e Public exponent.
 * @param[out] d Private key.
 * @return Message indicating success or failure.
 */
msg RSA_KeyGEN(OUT bigint** n, OUT bigint** e, OUT bigint** d);

/**
 * @brief Encrypts a message using RSA.
 * @param[out] c Encrypted message (ciphertext).
 * @param[in] m Original message (plaintext).
 * @param[in] e Public exponent.
 * @param[in] N Modulus.
 * @return Message indicating success or failure.
 */
msg RSA_ENC(OUT bigint** c, IN bigint** m, IN bigint** e, IN bigint** N);

/**
 * @brief Decrypts a ciphertext using RSA.
 * @param[out] m Decrypted message (plaintext).
 * @param[in] c Encrypted message (ciphertext).
 * @param[in] d Private key.
 * @param[in] N Modulus.
 * @return Message indicating success or failure.
 */
msg RSA_DEC(OUT bigint** m, IN bigint** c, IN bigint** d, IN bigint** N);

/**
 * @brief Generates RSA keys using the Chinese Remainder Theorem (CRT).
 * @param[out] n Public key N (modulus).
 * @param[out] p Prime number p.
 * @param[out] q Prime number q.
 * @param[out] e Public exponent.
 * @param[out] d_p CRT private key for p.
 * @param[out] d_q CRT private key for q.
 * @param[out] q_inv Modular inverse of q with respect to p.
 * @return Message indicating success or failure.
 */
msg RSA_CRT_KeyGEN(OUT bigint** n, OUT bigint** p, OUT bigint** q, OUT bigint** e, OUT bigint** d_p, OUT bigint** d_q, OUT bigint** q_inv);

/**
 * @brief Decrypts a ciphertext using RSA and the Chinese Remainder Theorem (CRT).
 * @param[out] m Decrypted message (plaintext).
 * @param[in] n Modulus.
 * @param[in] c Encrypted message (ciphertext).
 * @param[in] p Prime number p.
 * @param[in] q Prime number q.
 * @param[in] d_p CRT private key for p.
 * @param[in] d_q CRT private key for q.
 * @param[in] q_inv Modular inverse of q with respect to p.
 * @return Message indicating success or failure.
 */
msg RSA_CRT_DEC(OUT bigint** m, IN bigint** n, IN bigint** c, IN bigint** p, IN bigint** q, IN bigint** d_p, IN bigint** d_q, IN bigint** q_inv);

#endif
