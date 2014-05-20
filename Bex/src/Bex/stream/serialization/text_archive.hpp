#ifndef __BEX_STREAM_SERIALIZATION_TEXT_ARCHIVE__
#define __BEX_STREAM_SERIALIZATION_TEXT_ARCHIVE__

#include "text_iarchive.hpp"
#include "text_oarchive.hpp"

//////////////////////////////////////////////////////////////////////////
/// –Ú¡–ªØ in/out

namespace Bex { namespace serialization
{
    class text_archive
        : public text_iarchive
        , public text_oarchive
    {
    public:
        explicit text_archive(std::streambuf& sb, archive_mark state = default_mark)
            : text_iarchive(sb, state), text_oarchive(sb, state)
        {
        }

        explicit text_archive(std::iostream& ios, archive_mark state = default_mark)
            : text_iarchive(ios, state), text_oarchive(ios, state)
        {
        }

        using text_iarchive::load;
        using text_oarchive::save;
    };
} //namespace serialization

namespace {
    using serialization::text_archive;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_TEXT_ARCHIVE__