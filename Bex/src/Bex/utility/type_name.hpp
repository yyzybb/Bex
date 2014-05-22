#ifndef __BEX_UTILITY_TYPE_NAME_HPP__
#define __BEX_UTILITY_TYPE_NAME_HPP__

//////////////////////////////////////////////////////////////////////////
/// 获取类型的真实名字

#include <string>
//#include <boost/assert.hpp>

#if defined(__GNUC__)
#include <cxxabi.h>
#include <stdlib.h>
#endif 

namespace Bex
{
    template <typename T>
    struct real_typename_helper {};

    template <typename T>
    std::string real_typename()
    {
#if defined(__GNUC__)
        /// gcc.
        int s;
        char * realname = abi::__cxa_demangle(typeid(real_typename_helper<T>).name(), 0, 0, &s);
        std::string result(realname);
        free(realname);
#else
        std::string result(typeid(real_typename_helper<T>).name());
#endif 
        std::size_t start = result.find_first_of('<') + 1;
        std::size_t end = result.find_last_of('>');
        //BOOST_ASSERT((start < end && end <= result.size()));
        return result.substr(start, end - start);
    }

} // namespace Bex

#endif //__BEX_UTILITY_TYPE_NAME_HPP__