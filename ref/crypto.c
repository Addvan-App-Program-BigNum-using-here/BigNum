#include "crypto.h"

// BITS_SIZE는 헤더파일에서 수정 -> 나중에 변수로 코드 변경 필요
msg RSA(OUT bigint** m2, IN bigint** m){
    msg result_msg = RSA_FAIL;

    bigint* N = NULL;
    bigint* e = NULL;
    bigint* d = NULL;
    bigint* c = NULL;

    // key 생성
    result_msg = RSA_KeyGEN(&N, &e, &d);
    if(result_msg != RSA_KEYGEN_SUCCESS)   goto EXIT_RSA;

    // 암호화
    result_msg = RSA_ENC(&c, m, &e, &N);
    if(result_msg != RSA_ENC_SUCCESS)    goto EXIT_RSA;

    // 복호화
    result_msg = RSA_DEC(m2, &c, &d, &N);
    if(result_msg != RSA_DEC_SUCCESS)    goto EXIT_RSA;

    if(bi_compare(m, m2) != 0){
        result_msg = RSA_MISSMATCH;
        goto EXIT_RSA;
    }

    result_msg = RSA_SUCCESS;
EXIT_RSA:
    bi_delete(&N);
    bi_delete(&e);
    bi_delete(&d);
    bi_delete(&c);
    return result_msg;
}

msg RSA_CRT(OUT bigint** m2, IN bigint** m){
    msg result_msg = RSA_CRT_FAIL;

    bigint* N = NULL;
    bigint* p = NULL;
    bigint* q = NULL;
    bigint* e = NULL;
    bigint* d_p = NULL;
    bigint* d_q = NULL;
    bigint* q_inv = NULL;
    bigint* c = NULL;

    // key 생성
    result_msg = RSA_CRT_KeyGEN(&N, &p, &q, &e, &d_p, &d_q, &q_inv);
    if(result_msg != RSA_KEYGEN_SUCCESS)   goto EXIT_RSA;

    // 암호화
    result_msg = RSA_ENC(&c, m, &e, &N);
    if(result_msg != RSA_ENC_SUCCESS)    goto EXIT_RSA;

    // 복호화
    result_msg = RSA_CRT_DEC(m2, &N, &c, &p, &q, &d_p, &d_q, &q_inv);
    if(result_msg != RSA_DEC_SUCCESS)    goto EXIT_RSA;

    if(bi_compare(m, m2) != 0){
        result_msg = RSA_MISSMATCH;
        goto EXIT_RSA;
    }

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
    // RSA--15360
     result_msg = bi_set_from_string(&p, "dc5e82012ecc8ab307c5107610a733812708f6b76971ecd7f88ad51d092f90bee79ca711a8dd3ac579bfa6c0ccccf7216ad2b1edcbf31699d638ce6ecc43c8f4516fd887543acdbece1e8aff5f256318d9f2d5003a7cc68665c8d1aa875f1b1d69f08f6378be99776a92f74e41f49515492b152dd504c641ddc8e0bd0106b122dedeeb4f72d65767e2e44e1032dd2490770c558a7dced28266150845cea0156f48b2654b708db1d361165e1dd4029b4bb68f9d02e7ac80dc821c5e833ea0683c3b486a057b8abd9a1dd75489ad7845d702eed7f42767b32c033ec36f3459972ffed081dc37df1d644df94f7440402e4549eff47f8d5525541723dfc9f6e324f7942b5fa0abab4e3598e6e47e3217a3cb81d2fc384bd8b946f7f6b19e1ab39416ee4e94cc94af9ff5504a02a33da54b985dc41b6284c9192ba135d7d4e0abc4a29bb1a1ab182aa7aaca7dcb294c00324b2ee85708450e2dd641430a9ce741fbc60eaddd20fc514f12344f306c84f71968f4a2a913899c4d18ba3cddd9395d3fbbbc62f1f890ff71125c49155358e080c652ef1552fe6acd6cf8623dc187d5ddc88bd55a9496c8f27f29a088a485355a989de2fdf4c61ca6980a02781328dc75958e3fc11adfc85a0218d9e46ac14088f550610f60160313640d269f52f24e48e847308800188f3895f51f06fac72b9b8c9c47a64b79b9ac4106f666ec76d25789844bd8e97045b52f1291362ba96219230601968439b5d362ce116425afec51442fdfc2a743d5606a1b7f6a540e051ec5df307b33179ad417bcf151db3829ed014faedfb5f2c2e8039a4cd2c25f7f27c4aaaa4c4e9d5271306f555a3c03bce6bd771817a68cb127443ff490dcfd00a9e67e99855b5eaa325d6e60cfc979faf034881df69f002f34be5617f2186cd15b5b8a59910cec654c78c14420c5395aff83e95e9929005531c6c0b670a715d4e38118f205b55b85eafe3a91520a6300cee5b9b324ae63587c8ac945a99308afff75dc0a717d7a12d3766fc53de687d9498da4f398fcb813fc5e237452f4742fe9d2bb7cd29aefc81efc64d032f246795ee01e3d0a1d0f1b89c0e827280dc401b020b299461e1ff71e9193f0c08909e5b942a88d5a6f66d156b42912225bbfc3844af06039d913c48bdd461f47b5e98e34a68221cbc33a4dba8bdedd58739d5c4d25768a76e09f3e028f6e3a4a23304054c673db0a4f14ae42b2e150f5af480f92cc12e2a78582c7ebae95c9df4d1d4bb40ade2a49745a3d04e2ec816b66e6fdfe9e7f4219a1b0dab519c709eef6244e9350fcc6bf747784963c17461b13c31148ef6a84702597dce059305ca15d933fda1b", 16);

    // RSA-2048
//    result_msg = bi_set_from_string(&p, "837850864afe0c2123f5a751ce9599d62a235bf914d8403f25c2e199f40c48dbbc34f1ac0331b0065688d8841017fe3607fa836378a9b749f30e88c3139ad17447b18fa90d5704ae449dff10110f476ea5bf2ea41b6c4c257484c954b5148c496295a628398b8e658b4a3f306ab6749014ba20ec63963336468afae9ec5f6cfd", 16);

    //RSA-1024
//    result_msg = bi_set_from_string(&p, "F7E3FB983AF7D3A71A47764627B6D14F972947581B85C758C93C53D341E6657B7D23C8622A4275335C882FAA5BC651751D180D889DEB566337FD4C3C72474099", 16);
    if(result_msg != BI_SET_STRING_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // RSA--15360
     result_msg = bi_set_from_string(&q, "8b1ac30de0a4d99342afeedcaf61fb29c1565960668cd58121b0ea8fdea68c78337e52a076f879564db61a58859995183adb265a0105345831c32dc1da2f851e06cf461992dcb0d8fddb214524ef8bbd93842e184f307ec79645a8dc82b460a69d930866d975be896dcc7a094b5f9ac496e4e937c00115f50ffe3404d79bbd3cbb97f9a385c935c993b01fc4eac00d06d13bddc1821cb5a8e7843a8905ae044f8850c32c8212e3da2445202a67e4a69366232d6e709438c79a5ac0135d14a238a14c02c1a0bdb7e935fa37ad2b5bc8ca43a0438de856398ed227c3d9e40b82a95f5876bd1fd0aafa70d63fdd7b1ff15f3fb9ad862bab7e71ad7df2ebb7fddcf1d090ec9ec483a1c5db8dad2245565bc78496a7be8609837d53220bc867d89c6f212359e364110d7e4b05b4d3d93e0ba76f5ef7e89faa1a8790b17419e29a5c6d79f15b4b587ff95bf61e43b76e8fb30df980e412196c115374bf62d89a072a53bba502be22f68a01063083c4b49f214f17387fd1aa808d6ae824a2092b063641bb97f663b5d7a128a693f156892f11289c0f08319495b97833cf9a01353e5cc35f548a8162d56a74c8eca086cd627fc9b1b354801e6e2774aa125ba0274edbdb2162531aa638fea01caea892200949f3d07b56660fc318e0ff5a2e05d22bef150bb2e5bc7f250e345e12a29e26cb7b783970040fbeff90fcd2b14e891cdfc0f23202c92e77d151fe41c62c4b98ddc334981ed680ed40fe3bef7d13517b09bd2a963fafba1c69b604737973ba406ed5380343ee2df25c7f3ef90a89266b8f194db32a397b699cf2e86eeaae4ca663a2175048fdeb21c79e4db9f9f1c79f610aa3523d7b92e40b40b9d4cbfbb81a5b807238c2de5d3aded17a799e6c3ff940ad10722a08fca32dd06528e6298b703b26567ca06546487bf8995fd5d64a43a8d0834046e284aa5f4d3eaa7b5ecfd8f820c281888738f30825069baf234b52d103e8effc8e9e886214cb0fa3c97ac1dc91b3ff6c187ade2126ab27080e0d46b2535cbb9256614fbe5956e70c07cc248d5eca7e8b00584935465a90fef0832ea36480f6a8924d45f1a5358a78ed382dbde322b3a91e38c2b1f1236b8f89cfc8c115f3d8be304c5ae7ef35879407ebd78f00f9c6c89485a5056bf0c8ca6ce474f01527fbbe6b7eb30e381b14da5c81fdc99092f8653d50802592b5ffa9615b189be76915663f2093e7d6a71d85b826fd8084b9660bb89d205a7e1c0be9030521d623dfcd4016782cfb979953f8ad23a18723c6531faf6459d3ecb8777c6844bfb48a056f8cf95d8cfd4dda20eeeac55ff84fc64727e9fb31068751ca2f4ea2c42330c5", 16);

    // RSA-2048
//    result_msg = bi_set_from_string(&q, "956d6496569cf52c5a40d3b3bfdfee0182d50b85f064b7a65a0071ddc8a936c632ea5f15bbd88f0e27aedb47b3ff2ace5b499769fbd7a7b582fd4c250d112b5a6773727060542ba8b728dd8a3a96d75a3d9482aceb8f070408de2a2019801af7c7e36900264bbdb9d5eae55d3fc1583e2b701e91c2af95e839826579191e932b", 16);

    //RSA-1024
//    result_msg = bi_set_from_string(&q, "E8D43DA2817294C72E56C9C5710531F141328FA74855EEE719EF505848D44389BF734BAF307DD3615F395141CA5930ABB69F378A378299AF80DB1E2F9CDD0CA3", 16);
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
        result_msg = bi_div(&temp, d, d, &phi_n, div_option);
        if(result_msg != BI_DIV_SUCCESS)    goto EXIT_RSA_KEYGEN;
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
    printf("RSA_ENC\n");
    printf("m: ");
    bi_print(m, 16);
    printf("e: ");
    bi_print(e, 16);
    printf("N: ");
    bi_print(N, 16);
    if(bi_compare(m, N) >= 0)    return TOO_LONG_MESSAGE;
    msg result_msg = RSA_ENC_FAIL;

    // m^e mod N 계산
    result_msg = bi_exp_L_TO_R(c, m, e, N);
    if(result_msg != BI_EXP_L_TO_R_SUCCESS)    return result_msg;

    result_msg = RSA_ENC_SUCCESS;
    return result_msg;
}

msg RSA_DEC(OUT bigint** m, IN bigint** c, IN bigint** d, IN bigint** N){
    if(bi_compare(c, N) >= 0)    return TOO_LONG_MESSAGE;
    msg result_msg = RSA_DEC_FAIL;

    result_msg = bi_exp_MS(m, c, d, N);
    if(result_msg != BI_EXP_MS_SUCCESS)    return result_msg;

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
    // RSA--15360
         result_msg = bi_set_from_string(p, "dc5e82012ecc8ab307c5107610a733812708f6b76971ecd7f88ad51d092f90bee79ca711a8dd3ac579bfa6c0ccccf7216ad2b1edcbf31699d638ce6ecc43c8f4516fd887543acdbece1e8aff5f256318d9f2d5003a7cc68665c8d1aa875f1b1d69f08f6378be99776a92f74e41f49515492b152dd504c641ddc8e0bd0106b122dedeeb4f72d65767e2e44e1032dd2490770c558a7dced28266150845cea0156f48b2654b708db1d361165e1dd4029b4bb68f9d02e7ac80dc821c5e833ea0683c3b486a057b8abd9a1dd75489ad7845d702eed7f42767b32c033ec36f3459972ffed081dc37df1d644df94f7440402e4549eff47f8d5525541723dfc9f6e324f7942b5fa0abab4e3598e6e47e3217a3cb81d2fc384bd8b946f7f6b19e1ab39416ee4e94cc94af9ff5504a02a33da54b985dc41b6284c9192ba135d7d4e0abc4a29bb1a1ab182aa7aaca7dcb294c00324b2ee85708450e2dd641430a9ce741fbc60eaddd20fc514f12344f306c84f71968f4a2a913899c4d18ba3cddd9395d3fbbbc62f1f890ff71125c49155358e080c652ef1552fe6acd6cf8623dc187d5ddc88bd55a9496c8f27f29a088a485355a989de2fdf4c61ca6980a02781328dc75958e3fc11adfc85a0218d9e46ac14088f550610f60160313640d269f52f24e48e847308800188f3895f51f06fac72b9b8c9c47a64b79b9ac4106f666ec76d25789844bd8e97045b52f1291362ba96219230601968439b5d362ce116425afec51442fdfc2a743d5606a1b7f6a540e051ec5df307b33179ad417bcf151db3829ed014faedfb5f2c2e8039a4cd2c25f7f27c4aaaa4c4e9d5271306f555a3c03bce6bd771817a68cb127443ff490dcfd00a9e67e99855b5eaa325d6e60cfc979faf034881df69f002f34be5617f2186cd15b5b8a59910cec654c78c14420c5395aff83e95e9929005531c6c0b670a715d4e38118f205b55b85eafe3a91520a6300cee5b9b324ae63587c8ac945a99308afff75dc0a717d7a12d3766fc53de687d9498da4f398fcb813fc5e237452f4742fe9d2bb7cd29aefc81efc64d032f246795ee01e3d0a1d0f1b89c0e827280dc401b020b299461e1ff71e9193f0c08909e5b942a88d5a6f66d156b42912225bbfc3844af06039d913c48bdd461f47b5e98e34a68221cbc33a4dba8bdedd58739d5c4d25768a76e09f3e028f6e3a4a23304054c673db0a4f14ae42b2e150f5af480f92cc12e2a78582c7ebae95c9df4d1d4bb40ade2a49745a3d04e2ec816b66e6fdfe9e7f4219a1b0dab519c709eef6244e9350fcc6bf747784963c17461b13c31148ef6a84702597dce059305ca15d933fda1b", 16);

        // RSA-2048
//        result_msg = bi_set_from_string(p, "837850864afe0c2123f5a751ce9599d62a235bf914d8403f25c2e199f40c48dbbc34f1ac0331b0065688d8841017fe3607fa836378a9b749f30e88c3139ad17447b18fa90d5704ae449dff10110f476ea5bf2ea41b6c4c257484c954b5148c496295a628398b8e658b4a3f306ab6749014ba20ec63963336468afae9ec5f6cfd", 16);

        //RSA-1024
//        result_msg = bi_set_from_string(p, "F7E3FB983AF7D3A71A47764627B6D14F972947581B85C758C93C53D341E6657B7D23C8622A4275335C882FAA5BC651751D180D889DEB566337FD4C3C72474099", 16);
        if(result_msg != BI_SET_STRING_SUCCESS)    goto EXIT_RSA_KEYGEN;

        // RSA--15360
         result_msg = bi_set_from_string(q, "8b1ac30de0a4d99342afeedcaf61fb29c1565960668cd58121b0ea8fdea68c78337e52a076f879564db61a58859995183adb265a0105345831c32dc1da2f851e06cf461992dcb0d8fddb214524ef8bbd93842e184f307ec79645a8dc82b460a69d930866d975be896dcc7a094b5f9ac496e4e937c00115f50ffe3404d79bbd3cbb97f9a385c935c993b01fc4eac00d06d13bddc1821cb5a8e7843a8905ae044f8850c32c8212e3da2445202a67e4a69366232d6e709438c79a5ac0135d14a238a14c02c1a0bdb7e935fa37ad2b5bc8ca43a0438de856398ed227c3d9e40b82a95f5876bd1fd0aafa70d63fdd7b1ff15f3fb9ad862bab7e71ad7df2ebb7fddcf1d090ec9ec483a1c5db8dad2245565bc78496a7be8609837d53220bc867d89c6f212359e364110d7e4b05b4d3d93e0ba76f5ef7e89faa1a8790b17419e29a5c6d79f15b4b587ff95bf61e43b76e8fb30df980e412196c115374bf62d89a072a53bba502be22f68a01063083c4b49f214f17387fd1aa808d6ae824a2092b063641bb97f663b5d7a128a693f156892f11289c0f08319495b97833cf9a01353e5cc35f548a8162d56a74c8eca086cd627fc9b1b354801e6e2774aa125ba0274edbdb2162531aa638fea01caea892200949f3d07b56660fc318e0ff5a2e05d22bef150bb2e5bc7f250e345e12a29e26cb7b783970040fbeff90fcd2b14e891cdfc0f23202c92e77d151fe41c62c4b98ddc334981ed680ed40fe3bef7d13517b09bd2a963fafba1c69b604737973ba406ed5380343ee2df25c7f3ef90a89266b8f194db32a397b699cf2e86eeaae4ca663a2175048fdeb21c79e4db9f9f1c79f610aa3523d7b92e40b40b9d4cbfbb81a5b807238c2de5d3aded17a799e6c3ff940ad10722a08fca32dd06528e6298b703b26567ca06546487bf8995fd5d64a43a8d0834046e284aa5f4d3eaa7b5ecfd8f820c281888738f30825069baf234b52d103e8effc8e9e886214cb0fa3c97ac1dc91b3ff6c187ade2126ab27080e0d46b2535cbb9256614fbe5956e70c07cc248d5eca7e8b00584935465a90fef0832ea36480f6a8924d45f1a5358a78ed382dbde322b3a91e38c2b1f1236b8f89cfc8c115f3d8be304c5ae7ef35879407ebd78f00f9c6c89485a5056bf0c8ca6ce474f01527fbbe6b7eb30e381b14da5c81fdc99092f8653d50802592b5ffa9615b189be76915663f2093e7d6a71d85b826fd8084b9660bb89d205a7e1c0be9030521d623dfcd4016782cfb979953f8ad23a18723c6531faf6459d3ecb8777c6844bfb48a056f8cf95d8cfd4dda20eeeac55ff84fc64727e9fb31068751ca2f4ea2c42330c5", 16);

//         RSA-2048
//        result_msg = bi_set_from_string(q, "956d6496569cf52c5a40d3b3bfdfee0182d50b85f064b7a65a0071ddc8a936c632ea5f15bbd88f0e27aedb47b3ff2ace5b499769fbd7a7b582fd4c250d112b5a6773727060542ba8b728dd8a3a96d75a3d9482aceb8f070408de2a2019801af7c7e36900264bbdb9d5eae55d3fc1583e2b701e91c2af95e839826579191e932b", 16);

        //RSA-1024
//        result_msg = bi_set_from_string(q, "E8D43DA2817294C72E56C9C5710531F141328FA74855EEE719EF505848D44389BF734BAF307DD3615F395141CA5930ABB69F378A378299AF80DB1E2F9CDD0CA3", 16);
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
    result_msg = bi_exp_MS(&m_p, c, d_p, p);
    if(result_msg != BI_EXP_MS_SUCCESS)    return result_msg;

    // m_q = c^d_q mod q
    result_msg = bi_exp_MS(&m_q, c, d_q, q);
    if(result_msg != BI_EXP_MS_SUCCESS)    return result_msg;

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