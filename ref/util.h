#include "data_type.h"
#include "error_msg.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

msg bi_new(bigint **dst, int word_len);
msg bi_delete(bigint **dst);
msg bi_refine(bigint *src);
msg bi_assign(bigint **dst, bigint *src);

msg bi_set_from_array(bigint **dst, int sign, int word_len, word *data, int endian);
msg bi_set_from_string(bigint **dst, char *int_str, int base);
msg String_Divide(char* int_str, word* a, int base);

msg bi_get_random(bigint **dst, int word_len);

msg bi_print(bigint *dst, int base);