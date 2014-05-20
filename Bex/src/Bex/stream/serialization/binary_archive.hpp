#ifndef __BEX_STREAM_SERIALIZATION_BINARY_ARCHIVE__
#define __BEX_STREAM_SERIALIZATION_BINARY_ARCHIVE__

#include "binary_iarchive.hpp"
#include "binary_oarchive.hpp"

//////////////////////////////////////////////////////////////////////////
/// –Ú¡–ªØ in/out

namespace Bex { namespace serialization
{
    class binary_archive
        : public binary_iarchive
        , public binary_oarchive
    {
    public:
        explicit binary_archive(std::streambuf& sb, archive_mark state = default_mark)
            : binary_iarchive(sb, state), binary_oarchive(sb, state)
        {
        }

        explicit binary_archive(std::iostream& ios, archive_mark state = default_mark)
            : binary_iarchive(ios, state), binary_oarchive(ios, state)
        {
        }

        using binary_iarchive::load;
        using binary_oarchive::save;
    };
} //namespace serialization

namespace {
    using serialization::binary_archive;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_SERIALIZATION_BINARY_ARCHIVE__