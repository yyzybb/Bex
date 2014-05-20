#ifndef __BEX_HTTP_SYNC_CLIENT__
#define __BEX_HTTP_SYNC_CLIENT__

#include <string>

namespace Bex
{
    class SyncHttpClient
    {
    public:
        SyncHttpClient();
        ~SyncHttpClient();

        std::string get(std::string const& url, std::string const& resource = "/");
        std::string post(std::string const& url, std::string const& content);

    private:
        class Impl;
        Impl * pImpl;
    };
}

#endif //__BEX_HTTP_SYNC_CLIENT__