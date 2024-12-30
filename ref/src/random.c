#include "random.h"

/*************************************************
* Name:        bi_get_random
*
* Description: New allocate bigint struct and fill it with random values
*
* Arguments:   - bigint** dst: pointer to bigint struct
*              - int word_len: length of bigint struct
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg bi_get_random(OUT bigint** dst, const IN int word_len) {
    if (word_len <= 0)   return BI_INVALID_LENGTH;
    if (*dst == NULL){
        if(bi_new(dst, word_len) != BI_ALLOC_SUCCESS)    return BI_ALLOC_FAIL;
    }else if((*dst != NULL && (*dst)->word_len != word_len)){
        if(bi_resize(dst, word_len) != BI_RESIZE_SUCCESS)    return BI_RESIZE_FAIL;
    }
    int result_msg = 0;

    // 부호 랜덤 설정
    result_msg = randombytes(&(*dst)->sign, 1);
    if(result_msg != GEN_RANDOM_BYTES_SUCCESS)  return result_msg;
    (*dst)->sign = (*dst)->sign & 1;

    result_msg = array_random((*dst)->a, word_len);
    if (result_msg != GEN_RANDOM_SUCCESS)
        return result_msg;

    if(bi_refine(dst) != BI_SET_REFINE_SUCCESS) return BI_SET_REFINE_FAIL;

    return BI_GET_RANDOM_SUCCESS;
}

/*************************************************
* Name:        get_random_bigint
*
* Description: New allocate bigints struct and fill it with random values
*
* Arguments:   - int count : bigint count
*              - ... : bigint** dst
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg get_random_bigint(int random_word_size, int word_size, int word_size_limit, int count, ...){
    msg result = BI_GET_RANDOM_FAIL;
    va_list args;
    va_start(args, count);
    byte temp[1] = {0};

    // bigint 할당 및 각 bigint에 대한 word_size 할당
    bigint*** bigints = malloc(count * sizeof(bigint**));
    int* test_word_sizes = malloc(count * sizeof(int));

    // 각 bigint에 할당할 사이즈 결정
    for (int i = 0; i < count; i++) {
        bigints[i] = va_arg(args, bigint**);
        if (random_word_size) {
            do {
                if (randombytes(temp, 1) != GEN_RANDOM_BYTES_SUCCESS) {
                    result = GEN_RANDOM_BYTES_FAIL;
                    goto cleanup;
                }
                test_word_sizes[i] = temp[0] % word_size_limit;
            } while (test_word_sizes[i] <= 0);
        } else  test_word_sizes[i] = word_size;
    }

    // bigint 랜덤 생성
    for (int i = 0; i < count; i++) {
        result = bi_get_random(bigints[i], test_word_sizes[i]);
        if (result != BI_GET_RANDOM_SUCCESS)    goto cleanup;
    }

    result = BI_GET_RANDOM_SUCCESS;

cleanup:
    va_end(args);
    free(bigints);
    free(test_word_sizes);
    return result;
}

/*************************************************
* Name:        array_random
*
* Description: Fill array with random values
*
* Arguments:   - word* dst: pointer to bigint struct
*              - int word_len: length of bigint struct
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg array_random(word* dst, int word_len) {
    if (dst == NULL || word_len <= 0)   return BI_INVALID_LENGTH;
    msg result_msg = 0;
    int byte_len = word_len * (WORD_BITS / sizeof(byte) / 8);

    result_msg = randombytes((byte*)dst, byte_len);
    if(result_msg != GEN_RANDOM_BYTES_SUCCESS)  return result_msg;

    return GEN_RANDOM_SUCCESS;
}

/*************************************************
 * Name:        randombytes
 *
 * Description: Fill random value to bytes array
 *
 * Arguments:   - byte* dst: pointer to bigint struct
 *              - int byte_len: length of bigint struct
 * Return:      - msg : message. SUCCESS or FAIL
 **************************************************/
msg randombytes(IN byte *dst, IN int byte_len){
#ifdef _WIN32
    // Windows 버전
    HCRYPTPROV hProvider = 0;
    if (!CryptAcquireContext(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)){
        return GEN_RANDOM_BYTES_FAIL;
    }

    if (!CryptGenRandom(hProvider, byte_len, dst)){
        CryptReleaseContext(hProvider, 0);
        return GEN_RANDOM_BYTES_FAIL;
    }

    CryptReleaseContext(hProvider, 0);
    return GEN_RANDOM_BYTES_SUCCESS;
#else
    // Unix 버전 (기존 코드)
    static int fd = -1;
    ssize_t ret;

    while (fd == -1){
        fd = open("/dev/urandom", O_RDONLY);
        if (fd == -1 && errno == EINTR)
            continue;
        else if (fd == -1)
            return GEN_RANDOM_BYTES_FAIL;
    }

    while (byte_len > 0){
        do{
            ret = read(fd, dst, byte_len);
        }while(dst[byte_len - 1] == 0);
        if (ret == -1 && errno == EINTR)
            continue;
        else if (ret == -1)
            return GEN_RANDOM_BYTES_FAIL;
        dst += ret;
        byte_len -= ret;
    }

    return GEN_RANDOM_BYTES_SUCCESS;
#endif
}
/*************************************************
* Name:        get_random_string
*
* Description: Create Random String with given length
*
* Arguments:   - char* str: return String
*              - int str_len : length of return String
*              - int base : base of return String
* Return:      - msg : message. SUCCESS or FAIL
**************************************************/
msg get_random_string(OUT char** str, IN int str_len, IN int base){
    if(str_len <= 0)    return RAND_LENGTH_INVALID;
    byte temp;

    // 각 진수별 사용할 문자 배열
    const char binary_chars[] = "01";
    const char decimal_chars[] = "0123456789";
    const char hex_chars[] = "0123456789abcdef";
    const char *chars;
    int chars_len;

    switch(base) {
        case 2:
            chars = binary_chars;
            chars_len = sizeof(binary_chars) - 1;
            str_len *= 32;
            break;
        case 10:
            chars = decimal_chars;
            chars_len = sizeof(decimal_chars) - 1;
            str_len *= 9; // 10자리로 하면 overflow 날 수도 있어서 (최대 : 4294967296)
            break;
        case 16:
            chars = hex_chars;
            chars_len = sizeof(hex_chars) - 1;
            str_len *= 8;
            break;
        default:
            if(str != NULL)    (*str)[0] = '\0';
            return RAND_STRING_INVALID;
    }

    if(*str == NULL){
        *str = (char*)calloc(str_len + 1, sizeof(char));
        if(*str == NULL) return MEM_NOT_ALLOC;
    }else{
         if((int)strlen(*str) < str_len + 1){ // 여기 뭔가 에러 날 수 있음.. 확인 다시 필요
             *str = (char*)realloc(*str, (str_len + 1) * sizeof(char));
             if(*str == NULL) return MEM_NOT_ALLOC;
         }
     }


    int str_idx = str_len;
    while(str_idx > 0) {
        if(randombytes(&temp, 1) != GEN_RANDOM_BYTES_SUCCESS)  return GEN_RANDOM_BYTES_FAIL;
        int random_index = (int)temp % chars_len;
        if(strncmp(&chars[random_index], "0", 1) == 0 && str_idx == str_len)    continue;
        (*str)[str_idx - 1] = chars[random_index];
        str_idx--;
    }
    (*str)[str_len] = '\0';

    return RAND_STRING_SUCCESS;
}