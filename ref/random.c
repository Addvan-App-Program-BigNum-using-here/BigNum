#include "random.h"

/*************************************************
* Name:        bi_get_random
*
* Description: New allocate bigint struct and fill it with random values
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - int word_len: length of bigint struct
**************************************************/
typedef uint8_t byte; // 8-bit �ڷ��� ����
msg bi_get_random(bigint** dst, int word_len) {
	bi_new(*dst, word_len); // word_len ���̸� ���� bigint �޸� �Ҵ�

	//cnt : �ʿ��� ������ ����
	int cnt = word_len * (sizeof(word) / sizeof(byte)); // �迭�� ������ 1byte�� ���� => �� word_len * 4�� cnt �ʿ�

	(*dst)->sign = rand() & 1;

	while (cnt > 0) {
		for (int i = 0; i < 4; i++) { // word �迭�� ũ�� : 32-bit, rand() & 0xff�� ũ�� : 8-bit => �� 4�� �ݺ�
			(*dst)->a[cnt] += (rand() & 0xff) << (8* i); 
		}

		cnt--;
	}

	bi_refine(*dst); // ���ʿ��� 0 �����ؼ� �޸� ����ȭ
}


msg array_rand(word* dst, int wordlen){
	
}

msg bi_get_random_ver2(bigint** dst, int word_len){
	(byte*)
}
int main()
{	
	srand(time(NULL));

	//puts("HELLO KMU!!");

	bigint a, b, c;

	bi_new(&a, 16);
	bi_new(&b, 16);
	bi_get_random(&a, 16);
	bi_get_random(&b, 16);

	/*bi_set_from_string(&a, "0x12321328378213", 16);
	bi_set_from_string(&b, "0x1ffffffff78213", 16);*/
	//bi_assign(&a, &b);
	return 0;
}