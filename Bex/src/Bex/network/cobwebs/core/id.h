#ifndef __BEX_NETWORK_COBWEBS_ID_H__
#define __BEX_NETWORK_COBWEBS_ID_H__

#include <boost/cstdint.hpp>
#include <Bex/config.hpp>
#if !defined(BEX_SUPPORT_CXX11)
# include <boost/detail/interlocked.hpp>
#else
# include <atomic>
#endif


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
#if !defined(BEX_SUPPORT_CXX11)
        static volatile long s_id;
        inline static boost::uint32_t create()
        {
            return (boost::uint32_t)(BOOST_INTERLOCKED_INCREMENT(&s_id));
        }
#else
        static std::atomic<long> s_id;
        inline static boost::uint32_t create()
        {
            return (boost::uint32_t)(s_id++);
        }
#endif 
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
#if !defined(BEX_SUPPORT_CXX11)
        static boost::uint64_t s_id;
#else
        static std::atomic<long long> s_id;
#endif
        static boost::uint64_t create();
    };

    id64 create_id64(boost::uint64_t id_);
    id32 create_id32(boost::uint32_t id_);

    typedef id32 group_id;
    typedef id64 session_id;

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_SESSION_ID_HPP__