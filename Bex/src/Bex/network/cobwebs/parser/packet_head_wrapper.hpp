#ifndef __BEX_NETWORK_COBWEBS_PACKET_HEAD_WRAPPER_HPP__
#define __BEX_NETWORK_COBWEBS_PACKET_HEAD_WRAPPER_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/type_traits.hpp>
#include <Bex/stream.hpp>

namespace Bex { namespace cobwebs
{
    struct packet_head_wrapper_base
    {
        virtual ~packet_head_wrapper_base() {}
        virtual std::size_t size_of() = 0;
        virtual std::size_t buf_size(char *) = 0;
        virtual void onPacket(char *, std::size_t) = 0;
    };
    typedef boost::shared_ptr<packet_head_wrapper_base> ph_wrap_ptr;

    template <class PacketHead>
    struct packet_head_wrapper
        : public packet_head_wrapper_base
    {
#if defined(BEX_COBWEBS_CHECK_PACKET_HEAD_POD)
        BOOST_STATIC_ASSERT_MSG(boost::is_pod<PacketHead>::value
            , "PacketHead must be a pod struct!");
#endif

        typedef boost::function<void(PacketHead*, static_streambuf &)> OnPacketFunc;
        OnPacketFunc m_OnPacket;

    public:
        explicit packet_head_wrapper(OnPacketFunc const& on_packet)
            : m_OnPacket(on_packet)
        {
        }

        virtual void onPacket(char * buf, std::size_t len)
        {
            const std::size_t uPacketHeadSize = sizeof(PacketHead);
            BOOST_ASSERT(len >= uPacketHeadSize);
            BOOST_ASSERT(buf);

            PacketHead* pPacketHead = reinterpret_cast<PacketHead*>(buf);
            static_streambuf ssb(buf + uPacketHeadSize, len - uPacketHeadSize, false);
            m_OnPacket(pPacketHead, ssb);
        }

        virtual std::size_t size_of()
        {
            return sizeof(PacketHead);
        }

        virtual std::size_t buf_size(char * buf)
        {
            BOOST_ASSERT(buf);
            PacketHead * pPacketHead = reinterpret_cast<PacketHead *>(buf);
            return pPacketHead->size();
        }
    };

    template <class PacketHead>
    inline ph_wrap_ptr create_packet_head_wrapper(
        typename packet_head_wrapper<PacketHead>::OnPacketFunc const& on_packet)
    {
        typedef boost::shared_ptr<packet_head_wrapper<PacketHead> > Ptr;
        Ptr ptr(new packet_head_wrapper<PacketHead>(on_packet));
        return boost::static_pointer_cast<packet_head_wrapper_base>(ptr);
    }

} //namespace cobwebs
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_PACKET_HEAD_WRAPPER_HPP__