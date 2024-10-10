/*
10-08
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h> // strlen() 함수 포함

//typedef unsigned int word; # int형은 플랫폼에 따라 32-bit인지 16-bit인지 달라질 수 있음.
typedef uint32_t word; // stdint 헤더파일 필요
typedef uint32_t msg;

typedef struct {
	int sign;
	int word_len;
	word* a;
}bigint;

// bigint <- array
msg bi_set_from_array(bigint** dst, int sign, int word_len, word* a);

/* str = 0x123qqppwq 
문자가 숫자가 아닌 경우 예외처리 필요*/
msg bi_set_from_string(bigint** dst, char* int_str, int base); // 몇 진수로 인식할 것인지에 대한 명시가 필요 

/* A = {1,2,3,0}
 배열의 마지막 원소는 non-zero라는 명시 필요*/ 
msg bi_get_random(bigint** dst, int word_len);

msg bi_print(bigint* dst, int base); // 10진수 출력 어려움

msg bi_new(bigint** dst, int word_len); // 메모리 빌리기
msg bi_delete(bigint** dst);

msg bi_refine(bigint* dst);
/* tmp <- x*/

msg bi_assign(bigint** dst, bigint* src);

msg bi_new(bigint** dst, int word_len) {
	// 이미 할당된 메모리가 있으면 메모리 해제
	if (*dst != NULL) {
		bi_delete(dst);
	}

	*dst = (bigint*)malloc(sizeof(bigint));
	if (dst == NULL) {
		printf("메모리 할당 실패\n");
		return 1;
	}

	(*dst)->a = (word*)calloc(word_len, sizeof(word)); // word 배열 저장 가능한 메모리 할당
}

msg bi_delete(bigint** dst) {
	// 이미 메모리가 해제되었으면 함수 종료
	if (*dst == 0) {
		return 1;
	}

	free((*dst)->a); // a가 가리키는 메모리 해제
	// 이미 해제된 메모리가 어떤 값을 가리키지 않게 하기 위해 초기화
	for (int i = 0; i < ((*dst)->word_len); i++) {
		(*dst)->a[i] = NULL;
	}

	free(*dst); // 구조체 dst가 가리키는 메모리 해제
	*dst = NULL; // 이미 해제된 메모리가 어떤 값을 가리키지 않게 하기 위해 초기화
}

msg bi_set_from_array(bigint** dst, int sign, int word_len, word* a) {
	bi_new(dst, word_len); // dst 구조체(word 배열 포함) 저장 가능한 메모리 할당

	(*dst)->sign = sign;
	(*dst)->word_len = word_len;
	//(*dst)->a = (word*)calloc(word_len, sizeof(word)); // word 배열 저장 가능한 메모리 할당
	for (int i = 0; i < word_len; i++) {
		(*dst)->a[i] = a[i];
	}
}

// 0~f
msg bi_set_from_string(bigint** dst, char* int_str, int base) {
	int str_len = 0;
	str_len = strlen(int_str); // strlen() : '/0'을 제외한 문자열의 길이

	/*
	문자열을 저장하기 위해 필요한 word 배열 최대 크기 => 이후 bi_refine() 함수 이용해 불필요한 메모리 해제
	*/
	(*dst)->word_len = ((4 * str_len) % 32) + 1; 

	bi_new(dst, (*dst)->word_len);
	//(*dst)->a = (word*)calloc(((*dst)->word_len), sizeof(word));

	//미완성
	if (base == 16) { 
		if (int_str[0] == 0 && int_str[1] == 'x') { // 입력이 0x로 시작하는 경우
			for (int i = 0; i < str_len-4; i++) { // int_str[0], int_str[1]은 각각 0, x이므로 그 다음부터 16진수변환
				if (int_str[str_len - 1 - i] >= 0 && int_str[str_len - 1 - i] <= 9) {
					(*dst)->a[(i % 7)] += int_str[str_len - 1 - i] << 4 * (i % 8);
				}
				else if (int_str[str_len - 1 - i] >= 'a' && int_str[str_len - 1 - i] <= 'f') {
					(*dst)->a[(i % 7)] += (int_str[str_len - 1 - i] - 'a' + 10) << 4 * (i % 8);
				}
				else if (int_str[str_len - 1 - i] >= 'A' && int_str[str_len - 1 - i] <= 'F') {
					(*dst)->a[(i % 7)] += (int_str[str_len - 1 - i] - 'A' + 10) << 4 * (i % 8);
				}
				else
					printf("16진수 입력이 아닙니다.\n");
					return 1; // 입력값이 0~f(F)가 아닌 경우 함수 종료
			}
		}
		// 입력이 0x로 시작하지 않는 경우
		else {
			for (int i = 0; i < str_len; i++) {

			}
		}
	}
	
	//미완성
	//if (base == 10) {
	//	for (int i = 0; i < str_len; i++) { //int_str[str_len] = '/0'이므로 str_len-1까지
	//		if (int_str[str_len - 1 - i] < 0 || int_str[str_len - 1 - i] > 9) {
	//			printf("10진수 입력이 아닙니다.\n");
	//			return 1; // 입력값이 0~10이 아닌 경우 
	//		}
	//			if (int_str[str_len - 1 - i] >= 0 && int_str[str_len - 1 - i] <= 9) {
	//				(*dst)->a[(i % 7)] = int_str[str_len - 1 - i] << 4 * (i % 8);
	//			}
	//	
	//	}
	//}
}

msg bi_assign(bigint** dst, bigint* src) {
	if (*dst != NULL) { // dst에 이미 값이 들어있으면 메모리 해제
		bi_delete(dst);
	}
	bi_new(dst, src->word_len); //src와 같은 크기로 dst 메모리 할당

	(*dst)->sign = src->sign; // 부호 복사

	// 배열 복사
	for (int i = 0; i < src->word_len; i++) {
		(*dst)->a[i] = src->a[i];
	}
}

msg bi_refine(bigint* dst) {
	if (dst == NULL) { // 
		return 1;
	}
	
	int new_wordlen = dst->word_len;
	while (new_wordlen > 1) {
		// 상위 word가 0이 아니면 반복문 탈출
		if (dst->a[new_wordlen] != 0) {
			break;
		}
		// 상위 word가 0이면 불필요한 메모리를 차지하고 있으므로 제거
		new_wordlen--;
	}

	// dst의 원래 길이와 new_wordlen이 다르다면, 불필요한 메모리를 차지하고 있으므로 메모리 재할당
	if (dst->word_len != new_wordlen) {
		dst->word_len = new_wordlen;
		dst->a = (word*)realloc(dst->a, sizeof(word) * new_wordlen);
	}
}

/* A = {1,2,3,0}
 배열의 마지막 원소는 non-zero라는 명시 필요*/
typedef uint8_t byte; // 8-bit 자료형 정의
msg bi_get_random(bigint** dst, int word_len) {
	bi_new(*dst, word_len); // word_len 길이를 갖는 bigint 메모리 할당

	//cnt : 필요한 랜덤값 개수
	int cnt = word_len * (sizeof(word) / sizeof(byte)); // 배열에 랜덤값 1byte씩 저장 => 총 word_len * 4의 cnt 필요

	while (cnt > 0) {
		for (int i = 0; i < 4; i++) { // word 배열의 크기 : 32-bit, rand() & 0xff의 크기 : 8-bit => 총 4번 반복
			(*dst)->a[cnt] += (rand() & 0xff) << (8* i); 
		}

		cnt--;
	}

	bi_refine(*dst); // 불필요한 0 제거해서 메모리 최적화
}

int main()
{
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