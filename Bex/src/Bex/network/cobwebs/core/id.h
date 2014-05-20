#ifndef __BEX_NETWORK_COBWEBS_ID_H__
#define __BEX_NETWORK_COBWEBS_ID_H__

#include <boost/detail/interlocked.hpp>
#include <boost/cstdint.hpp>

namespace Bex { namespace cobwebs
{
    struct id32
    {
        const boost::uint32_t id;

        id32()
            : id(create())
        {
        }

        inline boost::uint32_t value() const
        {
            return id;
        }

        bool operator<(id32 const& other) const
        {
            return id < other.id;
        }

    private:
        id32(boost::uint32_t id_)
            : id(id_)
        {
        }

        friend id32 create_id32(boost::uint32_t);

    private:
        static volatile long s_id;
        inline static boost::uint32_t create()
        {
            return (boost::uint32_t)(BOOST_INTERLOCKED_INCREMENT(&s_id));
        }
    };

    struct id64
    {
        const boost::uint64_t id;

        id64()
            : id(create())
        {
        }

        inline boost::uint64_t value() const
        {
            return id;
        }

        bool operator<(id64 const& other) const
        {
            return id < other.id;
        }

    private:
        id64(boost::uint64_t id_)
            : id(id_)
        {
        }

        friend id64 create_id64(boost::uint64_t);

    private:
        static boost::uint64_t s_id;
        static boost::uint64_t create();
    };

    id64 create_id64(boost::uint64_t id_);
    id32 create_id32(boost::uint32_t id_);

    typedef id32 group_id;
    typedef id64 session_id;

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_SESSION_ID_HPP__