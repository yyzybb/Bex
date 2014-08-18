#ifndef __BEX_STREAM_SERIALIZATION_TEXT_ARCHIVE__
#define __BEX_STREAM_SERIALIZATION_TEXT_ARCHIVE__

#include "text_iarchive.hpp"
#include "text_oarchive.hpp"

//////////////////////////////////////////////////////////////////////////
/// 序列化 in/out

namespace Bex { namespace serialization
{
    class text_archive
        : public text_iarchive
        , public text_oarchive
    {
        typedef text_iarchive in_type;
        typedef text_oarchive out_type;

    public:
        explicit text_archive(std::streambuf& sb)
            : in_type(sb), out_type(sb)
        {
        }

        explicit text_archive(std::iostream& ios)
            : in_type(ios), out_type(ios)
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
    using serialization::text_archive;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_TEXT_ARCHIVE__