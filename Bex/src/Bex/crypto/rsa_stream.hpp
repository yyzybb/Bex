#ifndef __BEX_CRYPTO_RSA_STREAM_HPP__
#define __BEX_CRYPTO_RSA_STREAM_HPP__

//////////////////////////////////////////////////////////////////////////
/// 利用openssl.RSA进行加解密和签名的工具

namespace Bex {

class rsa_stream
{
public:
    enum transfer_type {
        pri_encrypt,    ///< 私钥加密
        pri_decrypt,    ///< 私钥解密
        pub_encrypt,    ///< 公钥加密
        pub_decrypt,    ///< 公钥解密
        pri_sign,       ///< 私钥签名
        pub_verify,     ///< 公钥验签
    };

private:
    RSA * rsa_;
    transfer_type type_;
    boost::asio::basic_streambuf<> buf_;
    boost::asio::basic_streambuf<> out_;

public:
    rsa_stream() : rsa_(0), type_(pri_encrypt) {}

    rsa_stream(const char* filename, transfer_type type, pem_password_cb* pem_pwd = 0)
    {
        open(filename, type, pem_pwd);
    }

    ~rsa_stream()
    {
        close();
    }

    bool open(const char* filename, transfer_type type, pem_password_cb* pem_pwd = 0)
    {
        if (rsa_) 
            close();

        FILE * f = 0;
        fopen_s(&f, filename, "r");
        if (!f)
            return false;

        switch (type)
        {
        case pri_encrypt:
        case pri_decrypt:
        case pri_sign:
            rsa_ = ::PEM_read_RSAPrivateKey(f, 0, pem_pwd, 0);
            break;

        case pub_encrypt:
        case pub_decrypt:
        case pub_verify:
            rsa_ = ::PEM_read_RSA_PUBKEY(f, 0, pem_pwd, 0);
            break;

        default:
            break;
        }

        fclose(f);
        type_ = type;
        return is_open();
    }

    void close()
    {
        ::RSA_free(rsa_);
        rsa_ = 0;
        buf_.consume(buf_.size());
        out_.consume(out_.size());
    }

    bool is_open() const
    {
        return (rsa_ != 0);
    }

    bool encrypt() const
    {
        return (type_ == pri_encrypt || type_ == pub_encrypt);
    }

    template <typename Sequence>
    std::size_t sputn(Sequence const& seq)
    {
        std::size_t s = seq.size() * sizeof(typename Sequence::value_type);
        if (!s) return 0;

        buf_.sputn((char*)&seq[0], s);
        return s;
    }

    template <typename Sequence>
    std::size_t sgetn(Sequence & seq)
    {
        BOOST_STATIC_ASSERT(sizeof(typename Sequence::value_type) == 1);

        int rsa_len = ::RSA_size(rsa_);
        int ret_len = 0;

        while (buf_.size())
        {
            boost::asio::mutable_buffer out_data = out_.prepare(rsa_len);
            boost::asio::const_buffer in_data = buf_.data();

            unsigned char * pout = (unsigned char*)boost::asio::detail::buffer_cast_helper(out_data);
            unsigned char * pin = (unsigned char*)boost::asio::detail::buffer_cast_helper(in_data);

            int do_len = encrypt() ? (std::min<int>)(rsa_len - RSA_PKCS1_PADDING_SIZE, buf_.size()) : rsa_len;

            switch (type_)
            {
                // 加密
            case pri_encrypt:
                ret_len = ::RSA_private_encrypt(do_len, pin, pout, rsa_, RSA_PKCS1_PADDING);
                if (ret_len < 0)
                    return 0;
                break;

            case pub_encrypt:
                ret_len = ::RSA_public_encrypt(do_len, pin, pout, rsa_, RSA_PKCS1_PADDING);
                if (ret_len < 0)
                    return 0;
                break;

                // 解密
            case pri_decrypt:
                ret_len = ::RSA_private_decrypt(do_len, pin, pout, rsa_, RSA_PKCS1_PADDING);
                if (ret_len < 0)
                    return 0;
                break;

            case pub_decrypt:
                ret_len = ::RSA_public_decrypt(do_len, pin, pout, rsa_, RSA_PKCS1_PADDING);
                if (ret_len < 0)
                    return 0;
                break;

            default:
                return 0;
            }

            out_.commit(ret_len);
            buf_.consume(do_len);
        }

        const void * data = boost::asio::detail::buffer_cast_helper(out_.data());
        std::size_t s = out_.size();
        seq.resize(s);
        memcpy(&seq[0], data, s);
        out_.consume(s);
        return s;
    }

    rsa_stream& operator<<(std::string const& str)
    {
        sputn(str);
        return (*this);
    }

    rsa_stream& operator>>(std::string & str)
    {
        sgetn(str);
        return (*this);
    }

    template <typename Sequence>
    bool sign(Sequence & sign)
    {
        BOOST_STATIC_ASSERT(sizeof(typename Sequence::value_type) == 1);

        if (type_ != pri_sign) return false;

        unsigned char md[20] = {};
        SHA1((const unsigned char*)boost::asio::detail::buffer_cast_helper(buf_.data()), buf_.size(), md);

        boost::asio::mutable_buffer out_data = out_.prepare(::RSA_size(rsa_));
        unsigned char* sigbuf = (unsigned char*)boost::asio::detail::buffer_cast_helper(out_data);
        unsigned int siglen = boost::asio::detail::buffer_size_helper(out_data);
        if (!::RSA_sign(NID_sha1, md, sizeof(md), sigbuf, &siglen, rsa_))
            return false;

        sign.resize(siglen);
        memcpy(&sign[0], sigbuf, siglen);
        return true;
    }

    template <typename Sequence>
    bool verify(Sequence const& sign)
    {
        if (sign.empty()) return false;
        if (type_ != pub_verify) return false;

        unsigned char* sigbuf = (unsigned char*)&sign[0];
        unsigned int siglen = sign.size() * sizeof(typename Sequence::value_type);

        unsigned char* m = (unsigned char*)boost::asio::detail::buffer_cast_helper(buf_.data());
        unsigned int m_len = buf_.size();

        unsigned char md[20] = {};
        SHA1(m, m_len, md);

        return (::RSA_verify(NID_sha1, md, sizeof(md), sigbuf, siglen, rsa_) == 1);
    }
};

} //namespace Bex

#endif //__BEX_CRYPTO_RSA_STREAM_HPP__