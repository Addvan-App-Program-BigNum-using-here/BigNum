#ifndef CRYPTO_H
#define CRYPTO_H

#include "data_type.h"
#include "util.h"
#include "msg_control.h"
#include "operate.h"

msg RSA();

msg RSA_CRT();

msg RSA_KeyGEN(OUT bigint** n, OUT bigint** e, OUT bigint** d);

msg RSA_ENC(OUT bigint** c, IN bigint** m, IN bigint** e, IN bigint** N);

msg RSA_DEC(OUT bigint** m, IN bigint** c, IN bigint** d, IN bigint** N);

msg RSA_CRT_KeyGEN(OUT bigint**n, OUT bigint** p, OUT bigint** q, OUT bigint** e, OUT bigint** d_p, OUT bigint** d_q, OUT bigint** q_inv);

msg RSA_CRT_DEC(OUT bigint** m, IN bigint** n, IN bigint** c, IN bigint** p, IN bigint** q, IN bigint** d_p, IN bigint** d_q, IN bigint** q_inv);

#endif