#include "TestPCH.h"
#include <Bex/crypto/rsa_stream.hpp>
using namespace Bex::crypto;

BOOST_AUTO_TEST_SUITE(s_crypto)

/// ≤‚ ‘serialization–‘ƒ‹
BOOST_AUTO_TEST_CASE(t_rsa_stream)
{
    const char* pri_fn = "../../openssl/rsa/rsa.pri";
    const char* pub_fn = "../../openssl/rsa/rsa.pub";

    std::string sz = "abcdefghijklmnopqrstuvwxyz";
    std::string data;
    for (int i = 0; i < 1000; ++i)
        data += sz, std::transform(sz.begin(), sz.end(), sz.begin(), [](char ch){
            return (char)((ch + 1 - 'a') % 26 + 'a');
        });
    //data.resize(12);

    rsa_stream sign(pri_fn, rsa_stream::pri_sign);
    rsa_stream verify(pub_fn, rsa_stream::pub_verify);

    if (!sign.is_open() || !verify.is_open())
        return ;

    sign << data;
    std::string sign_str;
    sign.sign(sign_str);

    Dump("sign size: " << sign_str.length());

    verify << data;
    bool ok = verify.verify(sign_str);
    //Dump("verify ok: " << std::boolalpha << ok);
    BOOST_CHECK(ok);

    rsa_stream pri_enc(pri_fn, rsa_stream::pri_encrypt);
    rsa_stream pub_dec(pub_fn, rsa_stream::pub_decrypt);

    if (!pri_enc.is_open() || !pub_dec.is_open())
        return;

    pri_enc << data;
    std::string encryption;
    pri_enc >> encryption;

    Dump("encryption size: " << encryption.size());
    //Dump("encryption strlen: " << strlen(encryption.c_str()));

    pub_dec << encryption;
    std::string result;
    pub_dec >> result;

    //Dump(result);
    //Dump("is correct: " << std::boolalpha << (result == data));
    BOOST_CHECK((result == data));
    return ;

    // private key encrypt
    FILE * f = 0;
    fopen_s(&f, "e:\\rsa.pri", "r");
    if (!f)
        return ;

    RSA * private_key_rsa = ::PEM_read_RSAPrivateKey(f, 0, 0, 0);
    fclose(f);
    
    if (!private_key_rsa)
        return ;

    int rsa_len = ::RSA_size(private_key_rsa);
    int buf_len = 90000;
    unsigned char* buf = new unsigned char[buf_len];
    memset(buf, 0, buf_len);
    int ret = ::RSA_private_encrypt(rsa_len, (unsigned char*)data.c_str(), buf, private_key_rsa, RSA_NO_PADDING);
    Dump("rsa_len: " << rsa_len);
    Dump("ret: " << ret);
    Dump("strlen: " << strlen((const char*)buf));

    ::RSA_free(private_key_rsa);

    // public key decrypt
    fopen_s(&f, "e:\\rsa.pub", "r");
    if (!f)
        return ;

    RSA * public_key_rsa = ::PEM_read_RSA_PUBKEY(f, 0, 0, 0);
    fclose(f);

    if (!public_key_rsa)
        return ;

    rsa_len = ::RSA_size(public_key_rsa);
    unsigned char* ori = new unsigned char[buf_len];
    memset(ori, 0, buf_len);
    ret = ::RSA_public_decrypt(rsa_len, buf, ori, public_key_rsa, RSA_NO_PADDING);
    Dump(rsa_len);
    Dump(ret);
    Dump((char*)ori);

    delete[] buf;
    delete[] ori;
    RSA_free(public_key_rsa);
}

BOOST_AUTO_TEST_SUITE_END()