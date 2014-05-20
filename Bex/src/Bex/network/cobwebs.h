#ifndef __BEX_NETWORK_COBWEBS_H__
#define __BEX_NETWORK_COBWEBS_H__

#include "cobwebs/core/core.h"
#include "cobwebs/session/tcp_session.h"
#include "cobwebs/connector/tcp_connector.h"
#include "cobwebs/acceptor/tcp_acceptor.h"
#include "cobwebs/parser/tcp_packet_parser.h"
#include "cobwebs/tcp_client.hpp"

namespace Bex
{
    namespace
    {
        using cobwebs::core;
        using cobwebs::options;
        using cobwebs::Pool;
        using cobwebs::session_factory;
        using cobwebs::session_initialized;
        using cobwebs::tcp_session;
        using cobwebs::tcp_session_ptr;
        using cobwebs::tcp_connector;
        using cobwebs::tcp_acceptor;
        using cobwebs::tcp_client;

        using cobwebs::tcp_packet_parser;
        using cobwebs::create_packet_head_wrapper;

    } //namespace
} //namespace Bex

#endif //__BEX_NETWORK_COBWEBS_H__