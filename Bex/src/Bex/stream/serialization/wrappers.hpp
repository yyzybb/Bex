#ifndef __BEX_SERIALIZATION_WRAPPERS_HPP__
#define __BEX_SERIALIZATION_WRAPPERS_HPP__

#include <boost/mpl/print.hpp>

namespace Bex { namespace serialization
{
    //////////////////////////////////////////////////////////////////////////
    /// 二进制数据流包装类
    struct binary_wrapper
    {
        char *      m_data;
        std::size_t m_size;
    
        template <typename T>
        explicit binary_wrapper(T & t)
            : m_data((char*)&t), m_size(sizeof(T))
        {
#if !defined(BEX_SERIALIZATION_USE_POD_EXTEND)
            static_assert(std::is_pod<T>::value, "This type must has 'serialize' function, or to be a 'pod' struct.");
#endif //!defined(BEX_SERIALIZATION_USE_POD_EXTEND)
        }

        inline char * data()
        {
            return m_data;
        }

        inline std::size_t size()
        {
            return m_size;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    /// 文本数据流包装类
    template <typename T>
    struct text_wrapper
    {
        T & m_data;

        explicit text_wrapper(T & data)
            : m_data(data) 
        {
        }

        inline T & data()
        {
            return m_data;
        }
    };

} //namespace serialization
} //namespace Bex

#endif //__BEX_SERIALIZATION_WRAPPERS_HPP__