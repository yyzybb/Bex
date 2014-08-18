#ifndef __BEX_STREAM_SERIALIZATION_BINARY_ARCHIVE__
#define __BEX_STREAM_SERIALIZATION_BINARY_ARCHIVE__

#include "binary_iarchive.hpp"
#include "binary_oarchive.hpp"

//////////////////////////////////////////////////////////////////////////
/// 序列化 in/out

namespace Bex { namespace serialization
{
    template <typename Stream = std::streambuf>
    class binary_archive
        : public binary_iarchive<Stream>
        , public binary_oarchive<Stream>
    {
        typedef binary_iarchive<Stream> in_type;
        typedef binary_oarchive<Stream> out_type;

    public:
        template <typename Arg>
        explicit binary_archive(Arg& arg)
            : in_type(arg), out_type(arg)
        {
        }

        inline bool good() const
        {
            return in_type::good() && out_type::good();
        }

        inline void clear(bool rollback = true)
        {
            in_type::clear(rollback);
            out_type::clear(rollback);
        }
    };
} //namespace serialization

namespace {
    using serialization::binary_archive;

} //namespace
} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_BINARY_ARCHIVE__