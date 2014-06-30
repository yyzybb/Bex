#include <Bex/bexio/bexio.hpp>
#include <Bex/auto_link.h>
#include <iostream>

using namespace Bex::bexio;

void start_server()
{
    io_service ios;
    typedef basic_session<tcp_protocol<>, session_list_mgr<> > my_session;
    typedef basic_server<my_session> server;
    options opt = options::test();
    server s(ios, opt);
}

void start_client()
{
}

int main()
{
    std::cin.get();
    return 0;
}