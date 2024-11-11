#include "random.h"

msg bi_get_random(bigint** dst, int word_len){
    msg result_msg = 0;
    printf("%d\n", word_len);
	result_msg = bi_new(dst, word_len);
    if (result_msg != BI_ALLOC_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }

    if (check_seed() != 1){
        return BI_GET_RANDOM_FAIL; // GET_SEED_FAIL error_msg에 추가?
    }
    
    if(RAND_bytes((byte*)&(*dst)->sign, sizeof((*dst)->sign)) != 1){
        return BI_GET_RANDOM_FAIL;
    }
    (*dst)->sign = (*dst)->sign & 1;

    result_msg = array_rand((*dst)->a, word_len);
    if(result_msg != GEN_RANDOM_SUCCESS)
        return result_msg;

    result_msg = bi_refine(*dst);
    if(result_msg != BI_SET_REFINE_SUCCESS){
        print_log(result_msg);
        return result_msg;
    }

    return BI_GET_RANDOM_SUCCESS;
}


msg array_rand(word* dst, int word_len){
    int byte_len = 0;
    byte_len = word_len * (sizeof(word) / sizeof(byte)); 
    if (RAND_bytes((byte*)dst, byte_len) != 1) {
        return GEN_RANDOM_FAIL;
    }

    return GEN_RANDOM_SUCCESS;
}

int check_seed(void){
    if (RAND_status() == 0){
        unsigned char buf[64];
        unsigned char hash[SHA256_DIGEST_LENGTH];
        time_t current_time = time(NULL);  
        
        // 프로세스 ID: Windows와 Unix에서 다르게 가져오기
        #ifdef _WIN32
        DWORD pid = GetCurrentProcessId();  // Windows API
        #else
        pid_t pid = getpid();               // Unix API
        #endif

        // buf에 현재 시간, 프로세스 ID, 메모리 주소 등을 문자열 형태로 기록
        snprintf((char *)buf, sizeof(buf), "%ld%ld%p", (long)current_time, (long)pid, &buf);
        
        // buf에 기록된 데이터를 SHA-256 해시로 변환하여 고유한 무작위 데이터 생성
        SHA256(buf, sizeof(buf), hash); // 32byte 출력

        double entropy = sizeof(hash) / 2.0; // 엔트로피 추정량을 보수적으로 절반으로 계산
        RAND_add(hash, sizeof(hash), entropy);

        if (RAND_status() == 0) {
            return 0;
        }
        else{
            return 1;
        }
    }
    else{
        return 1;
    }
}
