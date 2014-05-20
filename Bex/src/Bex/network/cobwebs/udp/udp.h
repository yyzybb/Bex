#ifndef __BEX_NETWORK_COBWEBS_UDP_UDP_H__
#define __BEX_NETWORK_COBWEBS_UDP_UDP_H__

#include <boost/shared_ptr.hpp>

namespace Bex { namespace cobwebs
{
    class udp_base
    {
        class Impl;
        boost::shared_ptr<Impl> m_impl;

    public:
        explicit udp_base(int local_port = 0);
        explicit udp_base(std::string const& remote_ip, int remote_port, int local_port = 0);
        virtual ~udp_base();

        void set_remote(std::string const& remote_ip, int remote_port);

        void send(char const* buffer, unsigned int length);

        void on_receive(char const* buffer, unsigned int length);
    };

} //namespace Bex 
} //namespace cobwebs

#endif //__BEX_NETWORK_COBWEBS_UDP_UDP_H__