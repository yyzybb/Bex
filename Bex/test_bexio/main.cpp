#pragma warning(disable:4996)
#define _WIN32_WINNT 0x501
#include <Bex/bexio/bexio.hpp>
#include <Bex/auto_link.h>
#include <string>
#include <iostream>
using namespace Bex::bexio;

#define Dump(x) do { std::cout << x << std::endl; } while(0)

enum {
    t_simple = 0,       // 简单测试
    t_pingpong = 1,     // pingpong测试吞吐量
    t_multiconn = 2,    // 大量并发连接
};

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
        Dump(boost::this_thread::get_id() << " on disconnect");
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
    virtual void on_connect() BEX_OVERRIDE
    {
        Dump(boost::this_thread::get_id() << " on connect " << remote_endpoint());
        char buf[2048] = {1};
        send(buf, sizeof(buf));
    }

    virtual void on_receive(char const* data, std::size_t size) BEX_OVERRIDE
    {
        //Dump(boost::this_thread::get_id() << " size = " << size << " data:" << std::string(data, size));
        bool ok = send(data, size);
        if (!ok)
            Dump("send buffer full!");
    }
};

template <class Session>
void start_server()
{
    io_service ios;
    typedef basic_server<Session> server;
    options opt = options::test();
    server s(ios, opt);
    bool ok = s.startup(server::endpoint(ip::address::from_string("0.0.0.0"), 28087));
    if (!ok)
        Dump("server startup error: " << s.get_error_code().message());

    server::mstrand_service_type & core = use_service<server::mstrand_service_type >(ios);
    io_service::work worker(core.actor());
    core.run();
}

template <class Session>
void start_client()
{
    io_service ios;
    typedef basic_client<Session> client;
    options opt = options::test();
    client c(ios, opt);
    bool ok = c.connect(client::endpoint(ip::address::from_string("127.0.0.1"), 28087));
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
    io_service ios;
    typedef basic_client<Session> client;
    options opt = options::test();

    std::list<boost::shared_ptr<client> > clients;

    for (int i = 0; i < count; ++i)
    {
        shared_ptr<client> c(new client(ios, opt));
        clients.push_back(c);
        c->set_async_connect_callback(&on_connect_callback);
        bool ok = c->async_connect(client::endpoint(ip::address::from_string("127.0.0.1"), 28087));
        if (!ok)
            Dump("connect error: " << c->get_error_code().message());
    }

    client::mstrand_service_type & core = use_service<client::mstrand_service_type >(ios);
    io_service::work worker(core.actor());
    core.run();
}

int main()
{
    int input = 0;
    do 
    {
        std::cout << "请输入端类型(0:simple, 1:pingpong, 2:multiconn) / (0:server, 1:client):" << std::endl;
        std::cin >> input;

        int type = input / 10;
        int point = input % 10;
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
            if (point == 0)
                start_server<simple_session>();
            else
            {
                int count = 1;
                std::cout << "请输入客户端数量:" << std::endl;
                std::cin >> count;
                start_multi_client<simple_session>(count);
            }
            break;

        default:
            break;
        }
    } while (true);

    std::cin.get();
    return 0;
}
