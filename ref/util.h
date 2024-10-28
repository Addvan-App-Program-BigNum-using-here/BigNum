#ifndef UTIL_H
#define UTIL_H

#include "data_type.h"
#include "msg_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

msg bi_new(OUT bigint **dst, const IN int word_len);
msg bi_delete(OUT bigint **dst);
msg bi_refine(OUT bigint *src);
msg bi_assign(OUT bigint **dst, const IN bigint *src);

msg bi_set_from_array(OUT bigint **dst, const IN int sign, const IN int word_len, const IN word *data, const IN int endian);
msg bi_set_from_string(OUT bigint **dst, IN char *int_str, const IN int base);
msg String_Divide(OUT char* int_str, IN word* a, const IN int base);

msg bi_print(const IN bigint *dst, const IN int base);

#endif // UTIL_H