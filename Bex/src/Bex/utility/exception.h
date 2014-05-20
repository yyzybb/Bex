#ifndef __BEX_UTILITY_EXCEPTION_H__
#define __BEX_UTILITY_EXCEPTION_H__

#include <boost/exception/all.hpp>
#include <exception>

namespace Bex
{
#if defined(_MSC_VER)
    class exception
        : virtual public std::exception
        , virtual public boost::exception
    {
    public:
        exception() {}
        explicit exception(const char* const& error_info)
            : std::exception(error_info)
        {
        }
        explicit exception(const char* const& error_info, int error_code)
            : std::exception(error_info, error_code)
        {
        }
    };
#else
    class exception
        : virtual public std::exception
        , virtual public boost::exception
    {
        std::string m_info;

    public:
        exception() {}
        explicit exception(const char* const& error_info)
            : m_info(error_info)
        {
        }
        explicit exception(const char* const& error_info, int /*error_code*/)
            : m_info(error_info)
        {
        }

        virtual const char* what() const throw()
        {
            return m_info.c_str();
        }
    };
#endif

} //namespace Bex

#endif //__BEX_UTILITY_EXCEPTION_H__