#include "random.h"

msg bi_get_random(bigint** dst, int wordlen){
	bi_new(dst, wordlen);

    (*dst)->sign = rand() & 1; 
    array_rand(dst, wordlen);

    bi_refine(*dst);
}

msg array_rand(word* dst, int word_len){
	byte* p = (byte *) dst;
    int cnt = word_len * (sizeof(word) / sizeof(byte));
    while (cnt > 0)
    {
        *p = rand() & 0xff; 
        p++;
        cnt--;
    }
}

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
	srand(time(NULL)); 

	return 0;
}