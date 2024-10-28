#ifndef RANDOM_H
#define RANDOM_H

#include "msg_control.h"
#include "util.h"
#include <stdio.h>

msg bi_get_random(OUT bigint** dst, const IN int word_len);
msg get_array_random(OUT word *a, const IN int word_len);

#endif // RANDOM_H