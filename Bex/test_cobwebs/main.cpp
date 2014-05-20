#include "stdafx.h"
#include <Bex/auto_link.h>
#include "my_session.h"
#include <signal.h>
#include <boost/timer.hpp>

void send_thread()
{
    char buf[6000] = {};
    int len_list[] = {16, 32, 4097, 5024};
    int i = 0;

    while (1)
    {
        int len = len_list[i++ % (sizeof(len_list) / sizeof(int))];
        if (get_option().test(t_packet))
        {
            _stPacketHead * pPacket = reinterpret_cast<_stPacketHead*>(&buf[0]);
            pPacket->buf_size = len - sizeof(_stPacketHead);
        }

        BEX_GET_SIGNAL(0, 0)(buf, len);
        Bex::sys_sleep(1);
    }

    Dump(__FUNCTION__ << " end");
}

void on_connect(Bex::tcp_session_ptr ptr, boost::system::error_code const& ec
    , boost::shared_ptr<my_session>& client)
{
#ifndef TEST_MULTI_SESSION
    Dump(__FUNCTION__);
#endif 

    if (ec)
    {
        Dump("error: " << ec.message());
        return ;
    }

    client = boost::static_pointer_cast<my_session>(ptr);
}

void test()
{
    Bex::core::getInstance().startup();
    Bex::options opts;

#ifdef TEST_MULTI_SESSION
    opts.recvbufbytes = 32;
    opts.sendbufbytes = 32;
    opts.max_packet_size = 32;
    opts.listen_count = 10;
#endif 

    opts.sendbufoverflow_disconnect = false;
    opts.recvbufoverflow_disconnect = false;

    Bex::tcp_acceptor server(opts);
    Bex::tcp_client<my_session> tcpClient(opts);
    boost::shared_ptr<my_session> single_client;
    std::list<boost::shared_ptr<my_session> > client_list;

    int port = 41001;
    int type = 0;

    Dump("1.client 2.server 3.multi_client 4.sync_connect 5.auto_connect");    
    std::cin >> type;

    Dump("0x1.t_send 0x2.t_send_and_shutdown 0x4.t_echo 0x8.t_packet");
    std::cin >> get_option().test_mark;

    if (type == 1)
    {
        Bex::tcp_connector::getInstance().
            async_connect<my_session>(port, "localhost"
            , boost::bind(&on_connect, _1, _2, boost::ref(single_client)), opts);
    }
    else if (type == 2)
    {
        bool bOk = server.startup<my_session>(port);
        BOOST_ASSERT(bOk);
    }
    else if (type == 3)
    {
        Dump("请输入客户端数量:");
        int client_count = 100;
        std::cin >> client_count;

        client_list.resize(client_count);
        BOOST_AUTO(it, client_list.begin());
        for (; it != client_list.end(); ++it)
        {
            Bex::tcp_connector::getInstance().
                async_connect<my_session>(port, "localhost"
                , boost::bind(&on_connect, _1, _2, boost::ref(*it)), opts);
        }
    }
    else if (type == 4)
    {
        boost::timer bt;
        boost::system::error_code ec;
        single_client = boost::static_pointer_cast<my_session>(Bex::tcp_connector::getInstance().
            connect<my_session>(port, "localhost", ec, opts));
        if (ec)
            Dump("timeout=" << bt.elapsed() << " s, error=" << ec.message());
    }
    else if (type == 5)
    {
        tcpClient.start_auto_connect(port, "localhost");
    }
    else
    {
        Dump("Input Error!");
        return ;
    }

    boost::thread(boost::bind(&send_thread)).detach();

    while (1)
    {
        Bex::core::getInstance().run();
        Bex::sys_sleep(1);

#ifdef WIN32
        std::size_t session_count = server.get_sessions()->size();
        char buf[1024];
        sprintf_s(buf, sizeof(buf), "Test Cobwebs. (%u)", session_count);
        ::SetConsoleTitleA(buf);
#endif //WIN32

        if (single_client && !single_client->is_valid())
            single_client.reset();
    }
}

void handle_ctrl_c(int)
{
    Dump(__FUNCTION__);
    Bex::core::getInstance().get_pool().release_memory();
    signal(SIGINT, &handle_ctrl_c);
}

int main()
{
    signal(SIGINT, &handle_ctrl_c);
    test();
    system("pause");
    return 0;
}
