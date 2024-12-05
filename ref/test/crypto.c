#include "crypto.h"

int main(){
    msg result_msg = 0;
    for(int i = 0; i < 10; i++){
        printf("%dth test\n", i);
//        result_msg = RSA();
        result_msg = RSA_CRT();
        log_msg(result_msg);
    }
    return 0;
}

// BITS_SIZE는 헤더파일에서 수정 -> 나중에 변수로 코드 변경 필요
msg RSA(){
    msg result_msg = RSA_FAIL;

    bigint* N = NULL;
    bigint* e = NULL;
    bigint* d = NULL;
    bigint* c = NULL;
    bigint* m = NULL;
    bigint* m2 = NULL;

    // key 생성
    result_msg = RSA_KeyGEN(&N, &e, &d);
    if(result_msg != RSA_KEYGEN_SUCCESS)   goto EXIT_RSA;

    // 랜덤으로 메시지 생성 (사실 사용자가 임의로 넣는 메시지 입력 값에 대한 예외처리 해야 함) -> 수도 코드 2, 3번 코드
    // M < N 조건 만족해야 함 -> 임의로 설정
    result_msg = bi_get_random(&m, N->word_len - 1);
    if(result_msg != BI_GET_RANDOM_SUCCESS)    goto EXIT_RSA;
    m->sign = 0; // 메시지는 양수이기 때문에

    // 암호화
    result_msg = RSA_ENC(&c, &m, &e, &N);
    if(result_msg != RSA_ENC_SUCCESS)    goto EXIT_RSA;

    // 복호화
    result_msg = RSA_DEC(&m2, &c, &d, &N);
    if(result_msg != RSA_DEC_SUCCESS)    goto EXIT_RSA;

    if(bi_compare(&m, &m2) != 0){
        printf("M : ");
        bi_print(&m, 16);
        printf("M2 : ");
        bi_print(&m2, 16);
        result_msg = RSA_MISSMATCH;
        goto EXIT_RSA;
    }

    printf("M : ");
    bi_print(&m, 16);
    printf("M2 : ");
    bi_print(&m2, 16);

    result_msg = RSA_SUCCESS;

EXIT_RSA:
    bi_delete(&N);
    bi_delete(&e);
    bi_delete(&d);
    bi_delete(&c);
    bi_delete(&m);
    bi_delete(&m2);
    return result_msg;
}

msg RSA_CRT(){
    msg result_msg = RSA_CRT_FAIL;

    bigint* N = NULL;
    bigint* p = NULL;
    bigint* q = NULL;
    bigint* e = NULL;
    bigint* d_p = NULL;
    bigint* d_q = NULL;
    bigint* q_inv = NULL;
    bigint* c = NULL;
    bigint* m = NULL;
    bigint* m2 = NULL;

    // key 생성
    result_msg = RSA_CRT_KeyGEN(&N, &p, &q, &e, &d_p, &d_q, &q_inv);
    if(result_msg != RSA_KEYGEN_SUCCESS)   goto EXIT_RSA;

    // 랜덤으로 메시지 생성 (사실 사용자가 임의로 넣는 메시지 입력 값에 대한 예외처리 해야 함) -> 수도 코드 2, 3번 코드
    // M < N 조건 만족해야 함 -> 임의로 설정
    result_msg = bi_get_random(&m, N->word_len - 1);
    if(result_msg != BI_GET_RANDOM_SUCCESS)    goto EXIT_RSA;
    m->sign = 0; // 메시지는 양수이기 때문에

    // 암호화
    result_msg = RSA_ENC(&c, &m, &e, &N);
    if(result_msg != RSA_ENC_SUCCESS)    goto EXIT_RSA;

    // 복호화
    result_msg = RSA_CRT_DEC(&m2, &N, &c, &p, &q, &d_p, &d_q, &q_inv);
    if(result_msg != RSA_DEC_SUCCESS)    goto EXIT_RSA;

    if(bi_compare(&m, &m2) != 0){
        printf("M : ");
        bi_print(&m, 16);
        printf("M2 : ");
        bi_print(&m2, 16);
        result_msg = RSA_MISSMATCH;
        goto EXIT_RSA;
    }

    printf("M : ");
    bi_print(&m, 16);
    printf("M2 : ");
    bi_print(&m2, 16);

    result_msg = RSA_SUCCESS;

EXIT_RSA:
    bi_delete(&N);
    bi_delete(&p);
    bi_delete(&q);
    bi_delete(&e);
    bi_delete(&d_p);
    bi_delete(&d_q);
    bi_delete(&q_inv);
    bi_delete(&c);
    bi_delete(&m);
    bi_delete(&m2);
    return result_msg;
}



msg RSA_KeyGEN(OUT bigint** n, OUT bigint** e, OUT bigint** d){
    msg result_msg = RSA_KEYGEN_FAIL;

    bigint* p = NULL;
    bigint* q = NULL;
    bigint* phi_n = NULL;
    bigint* one = NULL;
    bigint* p_minus_1 = NULL;
    bigint* q_minus_1 = NULL;
    bigint* temp = NULL; // 이거 gcd 받는 건데 나중에 위에서 안쓰는 변수로 대체 가능
    bigint* gcd = NULL;


    // 1 생성
    result_msg = bi_new(&one, 1);
    if(result_msg != BI_ALLOC_SUCCESS)    goto EXIT_RSA_KEYGEN;
    one->a[0] = 1;

    // p, q 값 설정
    result_msg = bi_set_from_string(&p, "837850864afe0c2123f5a751ce9599d62a235bf914d8403f25c2e199f40c48dbbc34f1ac0331b0065688d8841017fe3607fa836378a9b749f30e88c3139ad17447b18fa90d5704ae449dff10110f476ea5bf2ea41b6c4c257484c954b5148c496295a628398b8e658b4a3f306ab6749014ba20ec63963336468afae9ec5f6cfd", 16);
    if(result_msg != BI_SET_STRING_SUCCESS)    goto EXIT_RSA_KEYGEN;
    result_msg = bi_set_from_string(&q, "956d6496569cf52c5a40d3b3bfdfee0182d50b85f064b7a65a0071ddc8a936c632ea5f15bbd88f0e27aedb47b3ff2ace5b499769fbd7a7b582fd4c250d112b5a6773727060542ba8b728dd8a3a96d75a3d9482aceb8f070408de2a2019801af7c7e36900264bbdb9d5eae55d3fc1583e2b701e91c2af95e839826579191e932b", 16);
    if(result_msg != BI_SET_STRING_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // p-1, q-1 계산
    result_msg = bi_sub(&p_minus_1, &p, &one);
    if(result_msg != BI_SUB_SUCCESS)    goto EXIT_RSA_KEYGEN;
    result_msg = bi_sub(&q_minus_1, &q, &one);
    if(result_msg != BI_SUB_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // phi_n 계산
    result_msg = bi_mul_karachuba(&phi_n, &p_minus_1, &q_minus_1, p_minus_1->word_len / mul_karachuba_ratio);
    if(result_msg != BI_MUL_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // n 생성
    result_msg = bi_mul_karachuba(n, &p, &q, p->word_len / mul_karachuba_ratio);
    if(result_msg != BI_MUL_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // e 생성
    result_msg = bi_set_from_string(e, "10001", 16);
    if(result_msg != BI_SET_STRING_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // 생성된 e가 gcd(phi_n, e) = 1을 만족해야 함. 현재 e는 10001로 소수이므로 만족, 나중에 임의의 p, q에 대해도 만족하도록 수정
    result_msg = bi_gcd(&gcd, e, &phi_n);
    if(result_msg != BI_GCD_SUCCESS)    goto EXIT_RSA_KEYGEN;

    result_msg = bi_compare(&gcd, &one);
    if(result_msg != 0)    goto EXIT_RSA_KEYGEN;
    // 여기에 나중에 만족 안할 경우 로직 구현, 현재는 만족하는 경우만 구현

    // d 생성
    result_msg = bi_EEA(&gcd, &temp, d, &phi_n, e);
    if(result_msg != BI_EEA_SUCCESS)    goto EXIT_RSA_KEYGEN;
    if((*d)->sign){
        result_msg = bi_add(d, d, &phi_n);
        if(result_msg != BI_ADD_SUCCESS)    goto EXIT_RSA_KEYGEN;
    }

    result_msg = RSA_KEYGEN_SUCCESS;

EXIT_RSA_KEYGEN:
    bi_delete(&p);
    bi_delete(&q);
    bi_delete(&phi_n);
    bi_delete(&one);
    bi_delete(&p_minus_1);
    bi_delete(&q_minus_1);
    bi_delete(&temp);
    bi_delete(&gcd);

    return result_msg;
}

msg RSA_ENC(OUT bigint** c, IN bigint** m, IN bigint** e, IN bigint** N){
    msg result_msg = RSA_ENC_FAIL;

    // m^e mod N 계산
    result_msg = bi_exp_L_TO_R(c, m, e, N);
    if(result_msg != BI_EXP_L_TO_R_SUCCESS)    return result_msg;

    result_msg = RSA_ENC_SUCCESS;
    return result_msg;
}

msg RSA_DEC(OUT bigint** m, IN bigint** c, IN bigint** d, IN bigint** N){
    if(bi_compare(c, N) >= 0)    return RSA_DEC_FAIL;
    msg result_msg = RSA_DEC_FAIL;

    result_msg = bi_exp_L_TO_R(m, c, d, N);
    if(result_msg != BI_EXP_L_TO_R_SUCCESS)    return result_msg;

    result_msg = RSA_DEC_SUCCESS;
    return result_msg;
}

msg RSA_CRT_KeyGEN(OUT bigint** n, OUT bigint** p, OUT bigint** q, OUT bigint** e, OUT bigint** d_p, OUT bigint** d_q, OUT bigint** q_inv){
    msg result_msg = RSA_KEYGEN_FAIL;

    bigint* d = NULL;
    bigint* phi_n = NULL;
    bigint* one = NULL;
    bigint* p_minus_1 = NULL;
    bigint* q_minus_1 = NULL;
    bigint* temp = NULL; // 이거 gcd 받는 건데 나중에 위에서 안쓰는 변수로 대체 가능
    bigint* gcd = NULL;


    // 1 생성
    result_msg = bi_new(&one, 1);
    if(result_msg != BI_ALLOC_SUCCESS)    goto EXIT_RSA_KEYGEN;
    one->a[0] = 1;

    // p, q 값 설정
    result_msg = bi_set_from_string(p, "837850864afe0c2123f5a751ce9599d62a235bf914d8403f25c2e199f40c48dbbc34f1ac0331b0065688d8841017fe3607fa836378a9b749f30e88c3139ad17447b18fa90d5704ae449dff10110f476ea5bf2ea41b6c4c257484c954b5148c496295a628398b8e658b4a3f306ab6749014ba20ec63963336468afae9ec5f6cfd", 16);
    if(result_msg != BI_SET_STRING_SUCCESS)    goto EXIT_RSA_KEYGEN;
    result_msg = bi_set_from_string(q, "956d6496569cf52c5a40d3b3bfdfee0182d50b85f064b7a65a0071ddc8a936c632ea5f15bbd88f0e27aedb47b3ff2ace5b499769fbd7a7b582fd4c250d112b5a6773727060542ba8b728dd8a3a96d75a3d9482aceb8f070408de2a2019801af7c7e36900264bbdb9d5eae55d3fc1583e2b701e91c2af95e839826579191e932b", 16);
    if(result_msg != BI_SET_STRING_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // p-1, q-1 계산
    result_msg = bi_sub(&p_minus_1, p, &one);
    if(result_msg != BI_SUB_SUCCESS)    goto EXIT_RSA_KEYGEN;
    result_msg = bi_sub(&q_minus_1, q, &one);
    if(result_msg != BI_SUB_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // phi_n 계산
    result_msg = bi_mul_karachuba(&phi_n, &p_minus_1, &q_minus_1, p_minus_1->word_len / mul_karachuba_ratio);
    if(result_msg != BI_MUL_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // n 생성
    result_msg = bi_mul_karachuba(n, p, q, (*p)->word_len / mul_karachuba_ratio);
    if(result_msg != BI_MUL_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // e 생성
    result_msg = bi_set_from_string(e, "10001", 16);
    if(result_msg != BI_SET_STRING_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // 생성된 e가 gcd(phi_n, e) = 1을 만족해야 함. 현재 e는 10001로 소수이므로 만족, 나중에 임의의 p, q에 대해도 만족하도록 수정
    result_msg = bi_gcd(&gcd, e, &phi_n);
    if(result_msg != BI_GCD_SUCCESS)    goto EXIT_RSA_KEYGEN;

    result_msg = bi_compare(&gcd, &one);
    if(result_msg != 0)    goto EXIT_RSA_KEYGEN;
    // 여기에 나중에 만족 안할 경우 로직 구현, 현재는 만족하는 경우만 구현

    // d 생성
    result_msg = bi_EEA(&gcd, &temp, &d, &phi_n, e);
    if(result_msg != BI_EEA_SUCCESS)    goto EXIT_RSA_KEYGEN;
    if(d->sign){
        result_msg = bi_add(&d, &d, &phi_n);
        if(result_msg != BI_ADD_SUCCESS)    goto EXIT_RSA_KEYGEN;
    }

    // d_p생성
    result_msg = bi_div(&temp, d_p, &d, &p_minus_1, 1);
    if(result_msg != BI_DIV_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // d_q 생성
    result_msg = bi_div(&temp, d_q, &d, &q_minus_1, 1);
    if(result_msg != BI_DIV_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // q_inv 생성
    result_msg = bi_EEA(&gcd, &temp, q_inv, p, q);
    if(result_msg != BI_EEA_SUCCESS)    goto EXIT_RSA_KEYGEN;
    if((*q_inv)->sign){
        result_msg = bi_add(q_inv, q_inv, p);
        if(result_msg != BI_ADD_SUCCESS)    goto EXIT_RSA_KEYGEN;
    }

    result_msg = RSA_KEYGEN_SUCCESS;

EXIT_RSA_KEYGEN:
    bi_delete(&phi_n);
    bi_delete(&one);
    bi_delete(&p_minus_1);
    bi_delete(&q_minus_1);
    bi_delete(&temp);
    bi_delete(&gcd);

    return result_msg;
}

msg RSA_CRT_DEC(OUT bigint** m, IN bigint** n, IN bigint** c, IN bigint** p, IN bigint** q, IN bigint** d_p, IN bigint** d_q, IN bigint** q_inv){
    if(bi_compare(c, n) >= 0)    return RSA_DEC_FAIL;
    msg result_msg = RSA_DEC_FAIL;

    bigint* m_p = NULL;
    bigint* m_q = NULL;
    bigint* temp = NULL;
    bigint* mul_temp = NULL;

    // m_p = c^d_p mod p
    result_msg = bi_exp_L_TO_R(&m_p, c, d_p, p);
    if(result_msg != BI_EXP_L_TO_R_SUCCESS)    return result_msg;

    // m_q = c^d_q mod q
    result_msg = bi_exp_L_TO_R(&m_q, c, d_q, q);
    if(result_msg != BI_EXP_L_TO_R_SUCCESS)    return result_msg;

    // temp = m_p - m_q
    result_msg = bi_sub(&temp, &m_p, &m_q);
    if(result_msg != BI_SUB_SUCCESS)    return result_msg;

    // m = q_inv * temp mod N
    int max_len = max(temp->word_len, (*q_inv)->word_len);
    result_msg = bi_mul_karachuba(&mul_temp, q_inv, &temp, max_len / mul_karachuba_ratio);
    if(result_msg != BI_MUL_SUCCESS)    return result_msg;
    result_msg = bi_div(&temp, &mul_temp, &mul_temp, n, 1);
    if(result_msg != BI_DIV_SUCCESS)    return result_msg;

    // m = q * temp mod N
    max_len = max((*q)->word_len, mul_temp->word_len);
    result_msg = bi_mul_karachuba(m, q, &mul_temp, max_len / mul_karachuba_ratio);
    if(result_msg != BI_MUL_SUCCESS)    return result_msg;
    result_msg = bi_div(&temp, m, m, n, 1);
    if(result_msg != BI_DIV_SUCCESS)    return result_msg;

    // m = m_q + m mod N
    result_msg = bi_add(m, m, &m_q);
    if(result_msg != BI_ADD_SUCCESS)    return result_msg;
    result_msg = bi_div(&temp, m, m, n, 1);
    if(result_msg != BI_DIV_SUCCESS)    return result_msg;

    result_msg = RSA_DEC_SUCCESS;
    return result_msg;
}