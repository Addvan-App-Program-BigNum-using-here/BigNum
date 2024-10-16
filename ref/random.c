#include "random.h"

/*************************************************
* Name:        bi_get_random
*
* Description: New allocate bigint struct and fill it with random values
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - int word_len: length of bigint struct
**************************************************/
msg bi_get_random(bigint** dst, int word_len) {
	int result_msg;

	result_msg = bi_new(dst, word_len);
	if (result_msg != BI_ALLOC_SUCCESS) {
        return result_msg;
    }

	int cnt = word_len * (sizeof(word) / sizeof(byte));

	while (cnt > 0) {
		for (int i = 0; i < 4; i++) {
			(*dst)->a[cnt] += (rand() & 0xff) << (8* i);
		}

		cnt--;
	}
	result_msg = bi_refine(*dst);
	if (result_msg != BI_SET_REFINE_SUCCESS) {
        return result_msg;
    }

    return RANDOM_SUCCESS;

}

/*************************************************
* Name:        bi_array_random
*
* Description: Fill array with random values
*
* Arguments:   - word* dst: pointer to bigint struct
*              - int word_len: length of bigint struct
**************************************************/
msg get_array_random(word *a, int word_len) {
    int cnt = word_len * (sizeof(word) / sizeof(byte));

    while (cnt > 0) {
        for (int i = 0; i < 4; i++) {
            a[cnt] += (rand() & 0xff) << (8* i);
        }

        cnt--;
    }

    return RANDOM_SUCCESS;
}