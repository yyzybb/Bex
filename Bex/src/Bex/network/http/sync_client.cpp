#include "sync_client.h"
#include <boost/asio.hpp>

namespace Bex
{
    class SyncHttpClient::Impl
    {
    public:
        typedef  boost::asio::ip::tcp::resolver  tcp_resolver;
        typedef  boost::asio::ip::tcp::socket    tcp_socket;
        typedef  boost::asio::streambuf          streambuf;

        Impl()
            : resolver_(m_ios), socket_(m_ios)
        {
        }
    
        bool connect(std::string const& server)
        {
            m_host = server;
            tcp_resolver::query query(server, "http");
            tcp_resolver::iterator endpoint_iterator = resolver_.resolve(query);
            tcp_resolver::iterator it = boost::asio::connect(socket_, endpoint_iterator);
            return (it != tcp_resolver::iterator());
        }

        void send(std::string const& server)
        {
            request_.pubseekoff(0, std::ios_base::beg);
            std::ostream request_stream(&request_);
            request_stream << server;
            boost::asio::write(socket_, request_);
        }

        std::string recv()
        {
            std::stringstream ss;
            response_.pubseekoff(0, std::ios_base::beg);
            //boost::asio::read_until(socket, response_, "\r\n\r\n");
            boost::system::error_code error;
            while (boost::asio::read(socket_, response_,
                boost::asio::transfer_at_least(1), error))
                ss << &response_;
            disconnect();
            return ss.str();
        }

        void disconnect()
        {
            socket_.close();
        }

    private:
        boost::asio::io_service   m_ios;
        tcp_resolver resolver_;
        tcp_socket   socket_;
        streambuf request_;
        streambuf response_;
        std::string m_host;       ///< Ô¶¶Ëhost¼ÇÂ¼
    };

    SyncHttpClient::SyncHttpClient()
    {
        pImpl = new Impl;
    }

    SyncHttpClient::~SyncHttpClient()
    {
        if (pImpl) delete pImpl, pImpl = 0;
    }

    std::string SyncHttpClient::get( std::string const& url, std::string const& resource /*= "/"*/ )
    {
        try
        {
            if (!pImpl->connect(url))
                return std::string("");

            std::stringstream ss;
            ss << "GET " << resource << " HTTP/1.1\r\n";
            ss << "Host: " << url << "\r\n";
            ss << "Accept: */*\r\n";
            ss << "Connection: close\r\n\r\n";

            pImpl->send(ss.str());
            return pImpl->recv();
        }
        catch(std::exception & err)
        {
            err;
            //std::cout << err.what() << std::endl;
            pImpl->disconnect();
        }
        return std::string("");
    }

    std::string SyncHttpClient::post( std::string const& url, std::string const& content )
    {
        try
        {
            if (!pImpl->connect(url))
                return std::string("");

            std::stringstream ss;
            ss << "POST / HTTP/1.1\r\n";
            ss << "Host: " << url << "\r\n";
            ss << "Accept: */*\r\n";
            ss << "Connection: close\r\n\r\n";
            ss << "Content-Length: " << content.length() << "\r\n";
            ss << content;    
            pImpl->send(ss.str());
            return pImpl->recv();
    
        }
        catch(std::exception & err)
        {
            err;
            //std::cout << err.what() << std::endl;
            pImpl->disconnect();
        }
        return std::string("");
    }
}
