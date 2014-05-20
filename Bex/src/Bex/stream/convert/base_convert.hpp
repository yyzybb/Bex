#ifndef __BEX_STREAM_CONVERT_BASE_CONVERT_HPP__
#define __BEX_STREAM_CONVERT_BASE_CONVERT_HPP__

#include <Bex/type_traits/class_info.hpp>
#include <Bex/stream/serialization/utility.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_base_of.hpp>

//////////////////////////////////////////////////////////////////////////
/// 数据转换器(可与serialization库配合使用)
namespace Bex { namespace convert
{
    /// convert空基类
    class empty_base_convert {};

    /// convert基类
    template <class Archive, class Convert>
    class basic_serialize_convert
        : public empty_base_convert
        , protected archive_traits<Archive>
    {
    public:
        typedef Archive archive_type;
        typedef archive_traits<Archive> traits;
        typedef typename traits::oper_tag oper_tag;

    private:
        inline Convert* getConvert()
        {
            return static_cast<Convert*>(this);
        }

    public:
        template <class Ar>
        void serialize(Ar & ar, const unsigned int)
        {
            ar & getConvert()->get(oper_tag());
        }
    };

    template <typename T, bool>
    struct convert_traits_helper
        : public archive_traits<typename T::archive_type>
    {
        static const bool is_convert = true;
    };

    template <typename T>
    struct convert_traits_helper<T, false>
        : public archive_traits<void>
    {
        static const bool is_convert = false;
    };

    template <typename T>
    struct convert_traits_impl
        : public convert_traits_helper<T,
            boost::is_base_of<empty_base_convert, T>::value>
    {
    };

    template <typename T>
    struct convert_traits
        : public convert_traits_impl<typename remove_all<T>::type>
    {
    };
} //namespace convert

namespace {
    using convert::basic_serialize_convert;
    using convert::convert_traits;
} //namespace

} //namespace Bex

#endif //__BEX_STREAM_CONVERT_BASE_CONVERT_HPP__