#ifndef __BEX_SINGLETON__
#define __BEX_SINGLETON__

#include <boost/noncopyable.hpp>

namespace Bex
{
    template <typename T>
    class singleton
        : public boost::noncopyable
    {
        struct object_creator
        {
            object_creator()
            {
                singleton<T>::getInstance();
            }
            inline void do_nothing() {}
        };

        static object_creator object_creator_;

    public:
        ~singleton()
        {
            getInstance();  ///< 防止VC链接器将未引用的全局变量消除.
        }

        static T& getInstance()
        {
            static T obj;
            object_creator_.do_nothing();
            return obj;
        }
    };

    template <typename T>
    typename singleton<T>::object_creator singleton<T>::object_creator_;
}

#endif //__BEX_SINGLETON__