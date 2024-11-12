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
    int result_msg;

    result_msg = bi_new(dst, word_len);
    if (result_msg != BI_ALLOC_SUCCESS) {
        log_msg(result_msg);
        return result_msg;
    }

    result_msg = randombytes(&(*dst)->sign, 1);
    if(result_msg != GEN_RANDOM_BYTES_SUCCESS)  return result_msg;

    (*dst)->sign = (*dst)->sign & 1;

    result_msg = array_random((*dst)->a, word_len);
    if(result_msg != GEN_RANDOM_SUCCESS)    return result_msg;

    result_msg = bi_refine(*dst);
    if(result_msg != BI_SET_REFINE_SUCCESS){
        log_msg(result_msg);
        return result_msg;
    }

    return BI_GET_RANDOM_SUCCESS;
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
    msg result_msg = 0;
    int byte_len = word_len * (sizeof(word) / sizeof(byte));

    if (dst == NULL || word_len <= 0) {
        return BI_INVALID_LENGTH;
    }

    result_msg = randombytes((byte*)dst, byte_len);
    if(result_msg != GEN_RANDOM_BYTES_SUCCESS)
        return result_msg;

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
msg randombytes(IN byte* dst, IN int byte_len){
#ifdef _WIN32
    // Windows 버전
    HCRYPTPROV hProvider = 0;
    if (!CryptAcquireContext(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return GEN_RANDOM_BYTES_FAIL;
    }

    if (!CryptGenRandom(hProvider, byte_len, dst)) {
        CryptReleaseContext(hProvider, 0);
        return GEN_RANDOM_BYTES_FAIL;
    }

    CryptReleaseContext(hProvider, 0);
    return GEN_RANDOM_BYTES_SUCCESS;
#else
    // Unix 버전 (기존 코드)
    static int fd = -1;
    ssize_t ret;

    while(fd == -1) {
        fd = open("/dev/urandom", O_RDONLY);
        if(fd == -1 && errno == EINTR)
            continue;
        else if(fd == -1)
            return GEN_RANDOM_BYTES_FAIL;
    }

    while(byte_len > 0) {
        ret = read(fd, dst, byte_len);
        if(ret == -1 && errno == EINTR)
            continue;
        else if(ret == -1)
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
msg get_random_string(OUT char* str, IN int str_len, IN int base){
    int str_idx = str_len;
    byte temp;

    // 각 진수별 사용할 문자 배열
    const char binary_chars[] = "01";
    const char decimal_chars[] = "0123456789";
    const char hex_chars[] = "0123456789abcdef";
    const char* chars;
    int chars_len;

    switch(base) {
        case 2:
            chars = binary_chars;
            chars_len = sizeof(binary_chars) - 1;
            break;
        case 10:
            chars = decimal_chars;
            chars_len = sizeof(decimal_chars) - 1;
            break;
        case 16:
            chars = hex_chars;
            chars_len = sizeof(hex_chars) - 1;
            break;
        default:
            str[0] = '\0';
            return RAND_STRING_INVALID;
    }

    while(str_idx > 0) {
        if(randombytes(&temp, 1) != GEN_RANDOM_BYTES_SUCCESS)  return GEN_RANDOM_BYTES_FAIL;

        int random_index = (int)temp % chars_len;
        if(strncmp(&chars[random_index], "0", 1) == 0 && str_idx == str_len)    continue;
        str[str_idx - 1] = chars[random_index];
        str_idx--;
    }

    str[str_len] = '\0';

    return RAND_STRING_SUCCESS;
}