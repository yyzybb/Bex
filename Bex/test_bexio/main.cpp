#pragma warning(disable:4996)
#define _WIN32_WINNT 0x501
#include <iostream>
#include <string>
#define Dump(x) do { std::cout << x << std::endl; } while(0)

#include <Bex/bexio/bexio.hpp>
#include <Bex/auto_link.h>
#include <Bex/utility/format.hpp>
using namespace Bex::bexio;


enum {
    t_simple = 0,       // 简单测试
    t_pingpong = 1,     // pingpong测试吞吐量
    t_multiconn = 2,    // 大量并发连接
    t_packet = 3,       // 解包测试
    t_tcp_shutdown = 4, // 测试优雅地关闭连接(发送后立即shutdown, 要保证对端可以接收完整, 不丢数据.)
    t_ssl_shutdown = 5, // 测试优雅地关闭连接(发送后立即shutdown, 要保证对端可以接收完整, 不丢数据.)
};

std::string remote_ip = "127.0.0.1";
volatile long s_count = 0;
volatile long s_obj_count = 0;
io_service ios;

class simple_session
    : public basic_session<tcp_protocol<> >
{
public:
    virtual void on_connect() BEX_OVERRIDE
    {
        Dump(boost::this_thread::get_id() << " on connect " << remote_endpoint());
        char buf[] = "Hello, I'm bexio!";
        send(buf, sizeof(buf));
    }

    virtual void on_disconnect(error_code const& ec) BEX_OVERRIDE
    {
        Dump(boost::this_thread::get_id() << " on disconnect, error:" << ec.message());
    }

    virtual void on_receive(char const* data, std::size_t size) BEX_OVERRIDE
    {
        Dump(boost::this_thread::get_id() << " size = " << size << " data:" << std::string(data, size));
    }
};

class pingpong_session
    : public simple_session
{
public:
    static pingpong_session * p;

    virtual void on_connect() BEX_OVERRIDE
    {
        Dump(boost::this_thread::get_id() << " on connect " << remote_endpoint());
        char buf[8000] = {1};
        send(buf, sizeof(buf));
        p = this;
    }

    virtual void on_receive(char const* data, std::size_t size) BEX_OVERRIDE
    {
        //Dump(boost::this_thread::get_id() << " size = " << size << " data:" << std::string(data, size));
        reply(data, size);
    }

    virtual void on_disconnect(error_code const& ec) BEX_OVERRIDE
    {
        Dump(boost::this_thread::get_id() << " on disconnect, error:" << ec.message());
    }

    bool reply(char const* data, std::size_t size)
    {
        bool first = send(data, size);
        //bool second = send(data, size);
        return first;// || second;
    }
};
pingpong_session * pingpong_session::p = 0;

class multi_session
    : public basic_session<tcp_protocol<> >
{
public:
    multi_session()
    {
        ::BOOST_INTERLOCKED_INCREMENT(&s_obj_count);
    }

    ~multi_session()
    {
        ::BOOST_INTERLOCKED_DECREMENT(&s_obj_count);
    }

    virtual void on_connect() BEX_OVERRIDE
    {
        ::BOOST_INTERLOCKED_INCREMENT(&s_count);
    }

    virtual void on_disconnect(error_code const& ec) BEX_OVERRIDE
    {
        ::BOOST_INTERLOCKED_DECREMENT(&s_count);
    }
};


class packet_session
    : public basic_session<tcp_packet_protocol<> >
{
public:
    virtual void on_connect() BEX_OVERRIDE
    {
        Dump(boost::this_thread::get_id() << " on connect " << remote_endpoint());
        ::BOOST_INTERLOCKED_INCREMENT(&s_count);

        char p1[4] = {};
        char p2[8] = {};
        char p3[138] = {};
        char p4[4099] = {};
        *(boost::uint32_t*)p2 = 4;
        *(boost::uint32_t*)p3 = 134;
        *(boost::uint32_t*)p4 = 4095;

        send(p1, sizeof(p1));
        send(p2, sizeof(p2));
        send(p3, sizeof(p3));
        send(p4, sizeof(p4));
    }

    virtual void on_disconnect(error_code const& ec) BEX_OVERRIDE
    {
        Dump(boost::this_thread::get_id() << " on disconnect");
        ::BOOST_INTERLOCKED_DECREMENT(&s_count);
    }

    virtual void on_receive(error_code const& ec, boost::uint32_t * ph
        , char const* data, std::size_t size) BEX_OVERRIDE
    {
        //Dump(boost::this_thread::get_id() << " size = " << size << " data:" << std::string(data, size));
        if (ec)
        {
            Dump("error: " << ec.message());
            terminate();
            return ;
        }

        send((char const*)ph, size + sizeof(boost::uint32_t));
    }
};

template <typename Protocol>
class shutdown_session
    : public basic_session<Protocol>
{
public:
    shutdown_session()
    {
        ::BOOST_INTERLOCKED_INCREMENT(&s_obj_count);
    }

    ~shutdown_session()
    {
        ::BOOST_INTERLOCKED_DECREMENT(&s_obj_count);
    }

    virtual void on_connect() BEX_OVERRIDE
    {
        ::BOOST_INTERLOCKED_INCREMENT(&s_count);
        Dump(boost::this_thread::get_id() << " on connect " << remote_endpoint());
        char buf[] = "I will shutdown session!";
        bool ok = send(buf, sizeof(buf));
        shutdown();
        if (!ok)
            Dump("Send failed!");
    }

    virtual void on_receive(char const* data, std::size_t size) BEX_OVERRIDE
    {
        Dump("recv " << size << " bytes: " << std::string(data, size));
    }

    virtual void on_disconnect(error_code const& ec) BEX_OVERRIDE
    {
        ::BOOST_INTERLOCKED_DECREMENT(&s_count);
        Dump(boost::this_thread::get_id() << " on disconnect! error:" << ec.message());
    }
};

template <class Session>
void start_server()
{
    typedef basic_server<Session> server;
    options opt = options::test();
    if (boost::is_same<Session, multi_session>::value)
        opt.send_buffer_size = opt.receive_buffer_size = 64;

    server s(ios, opt);
    bool ok = s.startup(server::endpoint(ip::address::from_string("0.0.0.0"), 28087), 10);
    if (!ok)
        Dump("server startup error: " << s.get_error_code().message());

    server::mstrand_service_type & core = use_service<server::mstrand_service_type >(ios);
    io_service::work worker(core.actor());
    core.run();
}

template <class Session>
void start_client()
{
    typedef basic_client<Session> client;
    options opt = options::test();

    client c(ios, opt);
    bool ok = c.connect(client::endpoint(ip::address::from_string(remote_ip), 28087));
    if (!ok)
        Dump("connect error: " << c.get_error_code().message());

    client::mstrand_service_type & core = use_service<client::mstrand_service_type >(ios);
    io_service::work worker(core.actor());
    core.run();
}

void on_connect_callback(error_code const& ec)
{
    Dump("connected!");
}

template <class Session>
void start_multi_client(int count = 100)
{
    typedef basic_client<Session> client;
    options opt = options::test();
    if (boost::is_same<Session, multi_session>::value)
        opt.send_buffer_size = opt.receive_buffer_size = 64;

    std::list<boost::shared_ptr<client> > clients;

    for (int i = 0; i < count; ++i)
    {
        shared_ptr<client> c(new client(ios, opt));
        clients.push_back(c);
        c->set_async_connect_callback(&on_connect_callback);
        bool ok = c->async_connect(client::endpoint(ip::address::from_string(remote_ip), 28087));
        if (!ok)
            Dump("connect error: " << c->get_error_code().message());
    }

    client::mstrand_service_type & core = use_service<client::mstrand_service_type >(ios);
    io_service::work worker(core.actor());
    core.run();
}

void handle_ctrl_c(error_code, int, signal_set * ss)
{
    ss->async_wait(boost::BOOST_BIND(&handle_ctrl_c, _1, _2, ss));
    Dump("ctrl-c");
    char buf[10];
    if (pingpong_session::p)
    {
        bool ok = pingpong_session::p->send(buf, sizeof(buf));
        Dump("Send " << ok);
    }
}

int main()
{
    signal_set signal_proc(ios, SIGINT);
    signal_proc.async_wait(boost::BOOST_BIND(&handle_ctrl_c, _1, _2, &signal_proc));

    int input = 0;
    do 
    {
        std::cout << "请输入端类型(0:simple, 1:pingpong, 2:multiconn, 3:packet, 4:tcp_shutdown, 5:ssl_shutdown)"
            "\n\t(0:server, 1:client):" << std::endl;
        std::cin >> input;

        int type = input / 10;
        int point = input % 10;

#if defined(_MSC_VER) && (_MSC_VER >= 1800)
        boost::thread th([point] {
            for (;;)
            {
                long c = s_count;
                long oc = s_obj_count;
                ::SetConsoleTitleA(Bex::format("%s (%d)(%d)", ((point == 0) ? "server" : "client"), oc, c).c_str());
                boost::this_thread::sleep(boost::posix_time::millisec(100));
            }
        });
#endif

        switch (type)
        {
        case t_simple:
            if (point == 0)
                start_server<simple_session>();
            else
                start_client<simple_session>();
            break;

        case t_pingpong:
            if (point == 0)
                start_server<pingpong_session>();
            else
                start_client<pingpong_session>();
            break;

        case t_multiconn:
            {
                if (point == 0)
                    start_server<multi_session>();
                else
                {
                    int count = 1;
                    std::cout << "请输入客户端数量:" << std::endl;
                    std::cin >> count;
                    start_multi_client<multi_session>(count);
                }
            }
            break;

        case t_packet:
            {
                if (point == 0)
                    start_server<packet_session>();
                else
                {
                    int count = 1;
                    std::cout << "请输入客户端数量:" << std::endl;
                    std::cin >> count;
                    start_multi_client<packet_session>(count);
                }
            }
            break;

        case t_tcp_shutdown:
            if (point == 0)
                start_server<shutdown_session<tcp_protocol<> > >();
            else
                start_client<shutdown_session<tcp_protocol<> > >();
            break;

        case t_ssl_shutdown:
            if (point == 0)
                start_server<shutdown_session<tcp_protocol<> > >();
            else
                start_client<shutdown_session<tcp_protocol<> > >();
            break;

        default:
            break;
        }
    } while (true);

    std::cin.get();
    return 0;
}
