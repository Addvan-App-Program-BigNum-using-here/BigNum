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
//     result_msg = bi_set_from_string(&p, "de055ca6ed3d41d7c557c12c61285ad043d1a170733fee03c86dc3ca8fcc7cef1bf22ae63b52c798918e0daf392d62a0bd7ba53af6a1bd6c7da691a7966f61504b24992cf0039258349411447f0a7efbc00df0a9fb2a511df5c02d10367c6b4f8e158b1a7c9d030ae995f4297514653a7de16bb177f7dda58516a57f7d410e333744c69707a31e90d64ad9113e608a36879a94b9925b981b668a629c797c32e8a03f3e4f8e11c77fca08e3a904248b48429eb51be15af2e0c90d71118a30c121c10a06003a02169b052f61c9d529600160917d1add2025659f4cd98cfbcaf34a1929b843b28db282945436e446ded397c737cebc6372ce5ac627069380be7762f4860eca17b4142e551b33ebd18d0dbbde297f3b8adbcf26bef4f8227184b65759857d1e341024888bfd450e9b732fb2b4796b73dbdb67e76ed3b1a3a0677c85629426477e6e20a3264f524a8e1e09ea01dc6244f2ea918b5d74020bd9989f3affe2e0c99172a4c7171b6f08663c90319edcf83c7ccab2c8de50ac7b9231f94bbc0b92cd8f0183fb2e9569e30be864564cb11051266f024c1e69add5b2e9d83fdb6069f4094bac7752bd819a105fda47338fa3f2cf745c76ac79cacf3b4007a6a95caeeb0a54913c26a4585890f1756c33f0745525d4e5f1960521e6d9b66a9011882a1e727e2d23dfae7020123bd5ea75b42d057f4397d790a20c87751cbd00092b29b99376b6035d2273f0f64febe3b4b55e9960b84582c2e9a7dcb6c954cb386ca48c910929ad47b61b3c2502721c03246e9b169ca87acf9ff47512af09cb306493029ae9e42a8457464d1443b2556ba4c0b6e6fc1abff50f18c730e631ba2552be050fea9174d742d0c4e8b6c00ef9a467473ac6872be0928c2050e57bbaa3450e42f268b7dc4b67f0116d678acc04b75be0680e956cae4585b98217fdb09955972770ff8387ee5c6b40cfde625a35a1f254b026a25cd770434a743f279bd545fe24c99ecb3f8255953dbdf648ecc23a13f8d505796c6c28890c18d241323ff7df80c34af6937cee518b926bcce095867e90bc944738d11c1b4d07a472fea056b7d7a2378dc671251c8df01ba9cab43793b941d5f32211496bf44de516be694f5ecb7a7cd7ada794e5da05e8bcbc2872e6752ad0cff1b6443398a6e91a126d5875ae783ff4cda5823c636a1775c42455cde4405a29643e52b6704e755d192a44eb9fa01a465aacf2d0b9f507160f709f446c874b2d4ff0d2de3977c42c596af315d903bc3d105e1de11a259e53ee1f99677e11767a59b802cba501a8a4721719bd9bdbc783b2ca9f3bd58c0d3e7b391e0ecb440c04622a0d73411a02edb3", 16);

    // RSA-2048
    result_msg = bi_set_from_string(&p, "837850864afe0c2123f5a751ce9599d62a235bf914d8403f25c2e199f40c48dbbc34f1ac0331b0065688d8841017fe3607fa836378a9b749f30e88c3139ad17447b18fa90d5704ae449dff10110f476ea5bf2ea41b6c4c257484c954b5148c496295a628398b8e658b4a3f306ab6749014ba20ec63963336468afae9ec5f6cfd", 16);

    //RSA-1024
//    result_msg = bi_set_from_string(&p, "F7E3FB983AF7D3A71A47764627B6D14F972947581B85C758C93C53D341E6657B7D23C8622A4275335C882FAA5BC651751D180D889DEB566337FD4C3C72474099", 16);
    if(result_msg != BI_SET_STRING_SUCCESS)    goto EXIT_RSA_KEYGEN;

    // RSA--15360
//     result_msg = bi_set_from_string(&q, "8f1d7de72a28a1a66a188d5e1c8ee3f34feb41043915575d62de6631125706a7a1eb298db8e1470504af18b5c05b9476cbf13364c362050a1197b36acc858c72424bc0ab3ece292cdac7d5a985851fd34d0f9209683dfb2a266349c6b63bcc94d3d58ab3e8eb9c3b503ba8f693813fcb9b571359a1c66669739977a8506b4c407333d320896a75853ee01696a845bc3325265c733a3e269acee5a4408e41ea4fe15939b1fcbcf4e61ca5bde14105e7a07285218e0e1e759d7a8ce32bb1a2ef31a8a72498279c63fd45d0ea566f94c54acf3030d9cb96db32b122659b4168ff1c9a43f54a1cb0d59dbaa523b6a39a2e2aa02f200d1c64dbbf34f695301fcd5f106765265af7268d94cb8c56d421303c43a9688ac1c0c4d13a26d361e00e7da7c55b42cc184e9a0994e6fac23163e42b8e86e4a992a2535d4c261d2148b8180b0c990db9621a77cd16cd17bfafd32db442d6577cfb365921f2ce766f9b06ef3297d191e87f353ab09513dc49545ee845bdf7f663def680b7764b74a349cb574985a9269cdafdd5012817eacee0657f71a86e7533cf4182d61435f74ccc188d68d1c192ec96f345b310eec7494d0e0369be7d9836e954d90fcd7ef2c02ecdea30925f3e885f50f7304c0cdb79ffab090e105db93d3dcc8192100debd06a1fd738828244e77053e374d496f0c4b6df85768515bd8de6c7a9c4d0576147e4fcfa51e7850796eea2d1d2c62d06acec2bfaafff130cd4437f694c0a7d49a6bdffe0f2ed8d07ece5caf4f2258c8e7538ccade6a735e754bb1f4f3c9da8ff2a605e854f34df7e5350e0ff70d8dbac25f4db1d0880e7a2d55bca68068a72996818505514a8184f99bc8ac72e01077f1f41c89582dbe12722919c3c626176496b0b170b7a08e5598bcde8ceef1bc3c4f6f6d85d5477cb2aaed3f393f47d8764518162f5bd45608fe1aa33f3a25ba8b0f9a517c34246b8158c622bf6a6de9b177722d99228318d2adb9bc2e539f0d5340218cdb637842fbfcf42ed20340d992a59b37d2910bffc679c926660823e7470d6036e8993d253c43baad6f03bbcdf108667649c0087b2a7f18fc8deeb251d3b60bbca1c66061ec9808d85d0efe236b598042a617f9fa669b7a3ab33ebfea122bb2be99c6ed84c45d9f0b4ae1a077b8d54b5e088b92ca73616a649a865ce4d382e3ad9001922764be716f123f7e507a90bb801aab3ebed2357555190eafeac5fb618aa5a79a66420aeb2dd9f8232330e74af81b4d4952c186b0b58304030fee1a1f3e7a82d3b2f57aa9696b52d3bab919b0b301e57dd429a1e6eef107eccb0fad99542c0ca561296edf36630722d267c39f73ab91745", 16);

    // RSA-2048
    result_msg = bi_set_from_string(&q, "956d6496569cf52c5a40d3b3bfdfee0182d50b85f064b7a65a0071ddc8a936c632ea5f15bbd88f0e27aedb47b3ff2ace5b499769fbd7a7b582fd4c250d112b5a6773727060542ba8b728dd8a3a96d75a3d9482aceb8f070408de2a2019801af7c7e36900264bbdb9d5eae55d3fc1583e2b701e91c2af95e839826579191e932b", 16);

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
//         result_msg = bi_set_from_string(p, "dc5e82012ecc8ab307c5107610a733812708f6b76971ecd7f88ad51d092f90bee79ca711a8dd3ac579bfa6c0ccccf7216ad2b1edcbf31699d638ce6ecc43c8f4516fd887543acdbece1e8aff5f256318d9f2d5003a7cc68665c8d1aa875f1b1d69f08f6378be99776a92f74e41f49515492b152dd504c641ddc8e0bd0106b122dedeeb4f72d65767e2e44e1032dd2490770c558a7dced28266150845cea0156f48b2654b708db1d361165e1dd4029b4bb68f9d02e7ac80dc821c5e833ea0683c3b486a057b8abd9a1dd75489ad7845d702eed7f42767b32c033ec36f3459972ffed081dc37df1d644df94f7440402e4549eff47f8d5525541723dfc9f6e324f7942b5fa0abab4e3598e6e47e3217a3cb81d2fc384bd8b946f7f6b19e1ab39416ee4e94cc94af9ff5504a02a33da54b985dc41b6284c9192ba135d7d4e0abc4a29bb1a1ab182aa7aaca7dcb294c00324b2ee85708450e2dd641430a9ce741fbc60eaddd20fc514f12344f306c84f71968f4a2a913899c4d18ba3cddd9395d3fbbbc62f1f890ff71125c49155358e080c652ef1552fe6acd6cf8623dc187d5ddc88bd55a9496c8f27f29a088a485355a989de2fdf4c61ca6980a02781328dc75958e3fc11adfc85a0218d9e46ac14088f550610f60160313640d269f52f24e48e847308800188f3895f51f06fac72b9b8c9c47a64b79b9ac4106f666ec76d25789844bd8e97045b52f1291362ba96219230601968439b5d362ce116425afec51442fdfc2a743d5606a1b7f6a540e051ec5df307b33179ad417bcf151db3829ed014faedfb5f2c2e8039a4cd2c25f7f27c4aaaa4c4e9d5271306f555a3c03bce6bd771817a68cb127443ff490dcfd00a9e67e99855b5eaa325d6e60cfc979faf034881df69f002f34be5617f2186cd15b5b8a59910cec654c78c14420c5395aff83e95e9929005531c6c0b670a715d4e38118f205b55b85eafe3a91520a6300cee5b9b324ae63587c8ac945a99308afff75dc0a717d7a12d3766fc53de687d9498da4f398fcb813fc5e237452f4742fe9d2bb7cd29aefc81efc64d032f246795ee01e3d0a1d0f1b89c0e827280dc401b020b299461e1ff71e9193f0c08909e5b942a88d5a6f66d156b42912225bbfc3844af06039d913c48bdd461f47b5e98e34a68221cbc33a4dba8bdedd58739d5c4d25768a76e09f3e028f6e3a4a23304054c673db0a4f14ae42b2e150f5af480f92cc12e2a78582c7ebae95c9df4d1d4bb40ade2a49745a3d04e2ec816b66e6fdfe9e7f4219a1b0dab519c709eef6244e9350fcc6bf747784963c17461b13c31148ef6a84702597dce059305ca15d933fda1b", 16);

        // RSA-2048
        result_msg = bi_set_from_string(p, "837850864afe0c2123f5a751ce9599d62a235bf914d8403f25c2e199f40c48dbbc34f1ac0331b0065688d8841017fe3607fa836378a9b749f30e88c3139ad17447b18fa90d5704ae449dff10110f476ea5bf2ea41b6c4c257484c954b5148c496295a628398b8e658b4a3f306ab6749014ba20ec63963336468afae9ec5f6cfd", 16);

        //RSA-1024
//        result_msg = bi_set_from_string(p, "F7E3FB983AF7D3A71A47764627B6D14F972947581B85C758C93C53D341E6657B7D23C8622A4275335C882FAA5BC651751D180D889DEB566337FD4C3C72474099", 16);
        if(result_msg != BI_SET_STRING_SUCCESS)    goto EXIT_RSA_KEYGEN;

        // RSA--15360
//         result_msg = bi_set_from_string(q, "8b1ac30de0a4d99342afeedcaf61fb29c1565960668cd58121b0ea8fdea68c78337e52a076f879564db61a58859995183adb265a0105345831c32dc1da2f851e06cf461992dcb0d8fddb214524ef8bbd93842e184f307ec79645a8dc82b460a69d930866d975be896dcc7a094b5f9ac496e4e937c00115f50ffe3404d79bbd3cbb97f9a385c935c993b01fc4eac00d06d13bddc1821cb5a8e7843a8905ae044f8850c32c8212e3da2445202a67e4a69366232d6e709438c79a5ac0135d14a238a14c02c1a0bdb7e935fa37ad2b5bc8ca43a0438de856398ed227c3d9e40b82a95f5876bd1fd0aafa70d63fdd7b1ff15f3fb9ad862bab7e71ad7df2ebb7fddcf1d090ec9ec483a1c5db8dad2245565bc78496a7be8609837d53220bc867d89c6f212359e364110d7e4b05b4d3d93e0ba76f5ef7e89faa1a8790b17419e29a5c6d79f15b4b587ff95bf61e43b76e8fb30df980e412196c115374bf62d89a072a53bba502be22f68a01063083c4b49f214f17387fd1aa808d6ae824a2092b063641bb97f663b5d7a128a693f156892f11289c0f08319495b97833cf9a01353e5cc35f548a8162d56a74c8eca086cd627fc9b1b354801e6e2774aa125ba0274edbdb2162531aa638fea01caea892200949f3d07b56660fc318e0ff5a2e05d22bef150bb2e5bc7f250e345e12a29e26cb7b783970040fbeff90fcd2b14e891cdfc0f23202c92e77d151fe41c62c4b98ddc334981ed680ed40fe3bef7d13517b09bd2a963fafba1c69b604737973ba406ed5380343ee2df25c7f3ef90a89266b8f194db32a397b699cf2e86eeaae4ca663a2175048fdeb21c79e4db9f9f1c79f610aa3523d7b92e40b40b9d4cbfbb81a5b807238c2de5d3aded17a799e6c3ff940ad10722a08fca32dd06528e6298b703b26567ca06546487bf8995fd5d64a43a8d0834046e284aa5f4d3eaa7b5ecfd8f820c281888738f30825069baf234b52d103e8effc8e9e886214cb0fa3c97ac1dc91b3ff6c187ade2126ab27080e0d46b2535cbb9256614fbe5956e70c07cc248d5eca7e8b00584935465a90fef0832ea36480f6a8924d45f1a5358a78ed382dbde322b3a91e38c2b1f1236b8f89cfc8c115f3d8be304c5ae7ef35879407ebd78f00f9c6c89485a5056bf0c8ca6ce474f01527fbbe6b7eb30e381b14da5c81fdc99092f8653d50802592b5ffa9615b189be76915663f2093e7d6a71d85b826fd8084b9660bb89d205a7e1c0be9030521d623dfcd4016782cfb979953f8ad23a18723c6531faf6459d3ecb8777c6844bfb48a056f8cf95d8cfd4dda20eeeac55ff84fc64727e9fb31068751ca2f4ea2c42330c5", 16);

//         RSA-2048
        result_msg = bi_set_from_string(q, "956d6496569cf52c5a40d3b3bfdfee0182d50b85f064b7a65a0071ddc8a936c632ea5f15bbd88f0e27aedb47b3ff2ace5b499769fbd7a7b582fd4c250d112b5a6773727060542ba8b728dd8a3a96d75a3d9482aceb8f070408de2a2019801af7c7e36900264bbdb9d5eae55d3fc1583e2b701e91c2af95e839826579191e932b", 16);

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