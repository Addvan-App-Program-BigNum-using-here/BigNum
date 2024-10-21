#include "random.h"

/*************************************************
* Name:        bi_get_random
*
* Description: New allocate bigint struct and fill it with random values
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - int word_len: length of bigint struct
**************************************************/
msg bi_get_random(bigint** dst, int wordlen){
	bi_new(dst, wordlen);

    (*dst)->sign = rand() & 1; // DRBG로 교체 필요?
    array_rand(dst, wordlen);

    bi_refine(*dst);
}

/*************************************************
* Name:        array_rand
*
* Description: Fill word array with random values using for bi_get_random()
*
* Arguments:   - word* dst: pointer to word array
*              - int word_len: length of bigint struct
**************************************************/
msg array_rand(word* dst, int word_len){
	byte* p = (byte *) dst;
    int cnt = word_len * (sizeof(word) / sizeof(byte));
    while (cnt > 0)
    {
        *p = rand() & 0xff; //rand() DRBG로 교체 필요
        p++;
        cnt--;
    }
}

/*************************************************
* Name:        array_rand_ver2
*
* Description: Fill word array with random values using for bi_get_random()
*
* Arguments:   - word* dst: pointer to word array
*              - int word_len: length of bigint struct
**************************************************/
msg array_rand_ver2(word* dst, int word_len){
    int cnt = word_len * (sizeof(word) / sizeof(byte));

    while (cnt > 0) {
		for (int i = 0; i < (sizeof(word)/sizeof(byte)); i++) { 
			dst[cnt] += (rand() & 0xff) << (8* i); 
		}

		cnt--;
	}
}

int main()
{	
	srand(time(NULL)); // main 함수에서 srand 함수 호출

	return 0;
}