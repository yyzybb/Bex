#pragma warning(disable:4996)
#define _WIN32_WINNT 0x501
#include <Bex/bexio/bexio.hpp>
#include <Bex/auto_link.h>
#include <iostream>
using namespace Bex::bexio;

#define Dump(x) do { std::cout << x << std::endl; } while(0)

class session
    : public basic_session<tcp_protocol<> >
{
public:
    virtual void on_connect() BEX_OVERRIDE
    {
        Dump("on connect!");
        char buf[] = "Hello, I'm bexio!";
        send(buf, sizeof(buf));
    }

    virtual void on_disconnect(error_code const& ec) BEX_OVERRIDE
    {
        Dump("on disconnect");
    }

    virtual void on_receive(char const* data, std::size_t size) BEX_OVERRIDE
    {
        Dump("size = " << size << " data:" << data);
    }
};

void start_server()
{
    io_service ios;
    typedef basic_server<session> server;
    options opt = options::test();
    server s(ios, opt);
    bool ok = s.startup(server::endpoint(ip::address::from_string("0.0.0.0"), 8087));
    if (!ok)
        Dump("server startup error: " << s.get_error_code().message());

    session::mstrand_service_type & core = use_service<session::mstrand_service_type >(ios);
    io_service::work worker(core.actor());
    core.run();
}

void start_client()
{
}

int main()
{
    start_server();
    std::cin.get();
    return 0;
}
