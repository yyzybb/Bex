#include "stdafx.h"
#include "my_session.h"

my_session::my_session( socket_ptr pSock, Bex::session_initialized const& si )
    : tcp_session(pSock, si), m_receive_bytes(0), m_send_bytes(0), m_packet_count(0)
    , m_parser(si.opts->max_packet_size, Bex::create_packet_head_wrapper<_stPacketHead>(boost::bind(&my_session::on_packet, this, _1, _2)),
        boost::bind(&my_session::on_error_packet, this))
{
}

my_session::~my_session()
{
#ifndef TEST_MULTI_SESSION
    Dump(__FUNCTION__);
#endif 
}

/// 收到封包
void my_session::on_packet( _stPacketHead * pHead, Bex::static_streambuf & buf )
{
    ++ m_packet_count;
    //Dump(__FUNCTION__ << " packet_len=" << buf.size() + sizeof(_stPacketHead));
}

/// 封包解析出错
void my_session::on_error_packet()
{
    Dump(__FUNCTION__);
    abort();
}

void my_session::on_connected()
{
#ifndef TEST_MULTI_SESSION
    Dump(__FUNCTION__);
#endif 

    if (get_option().test(t_send))
    {
        m_sc = Bex::make_signal_scoped( BEX_GET_SIGNAL(0, 0).connect(
            boost::bind(&my_session::static_send, boost::weak_ptr<Bex::tcp_session>(shared_from_this()), _1, _2)) );
    }

    if (get_option().test(t_send_and_shutdown))
    {
        char buf[4097] = {};
        if (get_option().test(t_packet))
        {
            _stPacketHead * pPacketHead = reinterpret_cast<_stPacketHead *>(&buf[0]);
            pPacketHead->buf_size = sizeof(buf) - sizeof(_stPacketHead);
        }

        send(buf, sizeof(buf));
        shutdown();
        //close();
    }
}

bool my_session::send( char const* buf, std::size_t bytes )
{
    bool ret = Bex::tcp_session::send(buf, bytes);
    if (ret)
        m_send_bytes += bytes;
    return ret;
}

bool my_session::static_send( boost::weak_ptr<Bex::tcp_session> wp, char const* buf, std::size_t bytes )
{
    boost::shared_ptr<Bex::tcp_session> ptr = wp.lock();
    if (ptr)
    {
        return ptr->send(buf, bytes);
    }

    return false;
}

void my_session::on_recv( char * buf, std::size_t bytes )
{
    //Dump(bytes /*<< " " << std::string(buf, bytes)*/);
    m_receive_bytes += bytes;

    if (get_option().test(t_echo))
        send(buf, bytes);

    m_parser.sputn(buf, bytes);
}

void my_session::on_disconnect( boost::system::error_code const& ec )
{
    Dump(__FUNCTION__ << " error_code - val=" << ec.value() << ", msg=" << ec.message());
    Dump("recv = " << m_receive_bytes << ", send = " << m_send_bytes);
}

void my_session::recv_handler( boost::system::error_code const& ec, std::size_t bytes )
{
    //Dump(__FUNCTION__ << " bytes=" << bytes);
    Bex::tcp_session::recv_handler(ec, bytes);
}

void my_session::send_handler( boost::system::error_code const& ec, std::size_t bytes )
{
    //Dump(__FUNCTION__ << " bytes=" << bytes);
    Bex::tcp_session::send_handler(ec, bytes);
}

